#include "Components/TrainingEpisodeSetupComponent.h"

#include "PCGComponent.h"
#include "Data/LogChannels.h"
#include "Data/TrainingDataTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"

void UTrainingEpisodeSetupComponent::SetPCGComponent(UPCGComponent* InPCGComponent)
{
	PCGComponent = InPCGComponent;
	PCGComponent->OnPCGGraphCleanedDelegate.AddUObject(this, &UTrainingEpisodeSetupComponent::OnPCGCleanupCompleted);
	PCGComponent->OnPCGGraphGeneratedDelegate.AddUObject(this, &UTrainingEpisodeSetupComponent::OnPCGGenerateCompleted);
	
	FoundSpawnLocationDelegate.BindUObject(this, &UTrainingEpisodeSetupComponent::OnFoundSpawnLocation);
	FoundInitialLookAtLocationDelegate.BindUObject(this, &UTrainingEpisodeSetupComponent::OnFoundInitialLookAtLocation);
	FoundEpisodeOriginLocationDelegate.BindUObject(this, &UTrainingEpisodeSetupComponent::OnFoundEpisodeOriginLocation);
}

void UTrainingEpisodeSetupComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTrainingEpisodeSetupComponent::SetupEpisode()
{
	if (TrainingPresets.IsEmpty())
	{
		ensure(false);
		return;
	}
	
	if (bSetupInProgress)
		return;
	
	bSetupInProgress = true;
	Cleanup();
	if (bRandomizeSeedOnSetup)
		EpisodeSeed = FMath::Rand();
	
	if (bUseRandomEpisodeSetup)
		CurrentPresetIndex = FMath::RandRange(0, TrainingPresets.Num() - 1);
	else 
		CurrentPresetIndex = (CurrentPresetIndex + 1) % TrainingPresets.Num();
	
	for (auto& ActorSpawnDescriptor : TrainingPresets[CurrentPresetIndex].CharactersSpawnDescriptors)
	{
		ActorSpawnDescriptor.ResetSpawnLocations();
		for (const auto& SetupActionInstancedStruct : ActorSpawnDescriptor.SetupPipeline)
		{
			const auto& SetupAction = SetupActionInstancedStruct.Get<FSetupAction>();
			TUniquePtr<FExternalMemory> ExternalMemory = SetupAction.MakeMemory();
			if (ExternalMemory.IsValid())
				EpisodeSetupActionMemories.Add(SetupAction.PipelineActionId, MoveTemp(ExternalMemory));
		}
	}
	
	if (!PCGComponent.IsValid() || !PCGComponent->IsCleaningUp())
		FindEpisodeOriginLocation();
}

void UTrainingEpisodeSetupComponent::RepeatEpisode()
{
	if (bSetupInProgress)
		return;
	
	bSetupInProgress = true;
	DestroySpawnedCharacters();
	for (const auto& SpawnDescriptor : TrainingPresets[CurrentPresetIndex].CharactersSpawnDescriptors)
		SpawnCharacter(SpawnDescriptor, true);
}

void UTrainingEpisodeSetupComponent::DestroySpawnedCharacters()
{
	for (const auto& SpawnedPawn : SpawnedActors)
		SpawnedPawn->Destroy();
	
	SpawnedActors.Empty();
	CurrentSpawnIndex = 0;
}

void UTrainingEpisodeSetupComponent::Cleanup()
{
	if (PCGComponent->IsGenerating())
		PCGComponent->StopGenerationInProgress();

	// TODO cancel active EQSs
	
	DestroySpawnedCharacters();
	EpisodeSetupActionMemories.Reset();
	if (PCGComponent.IsValid())
		PCGComponent->Cleanup();
}

void UTrainingEpisodeSetupComponent::SpawnCharacter(const FMLTrainingCharacterSpawnDescriptor& SpawnDescriptor, bool bRepeatSetup)
{
	FRotator Rotation = (SpawnDescriptor.LastInitialLookAtLocation - SpawnDescriptor.LastSpawnLocation).GetSafeNormal().Rotation();
	auto WorldLocal = GetWorld();
	FTransform SpawnTransform = FTransform(Rotation, SpawnDescriptor.LastSpawnLocation);
	auto SpawnClass = SpawnDescriptor.PawnClass.LoadSynchronous();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	auto Pawn = SpawnDescriptor.bSpawnDeferred  
		? WorldLocal->SpawnActorDeferred<APawn>(SpawnClass, SpawnTransform, nullptr, nullptr, SpawnParameters.SpawnCollisionHandlingOverride)
		: WorldLocal->SpawnActor<APawn>(SpawnClass, SpawnDescriptor.LastSpawnLocation, Rotation, SpawnParameters);
	
	if (Pawn == nullptr)
	{
		UE_VLOG(this, LogNpcMl, Warning, TEXT("Failed to spawn pawn"));
		UE_VLOG_LOCATION(this, LogNpcMl, Warning, SpawnDescriptor.LastSpawnLocation, 25.f, FColor::Yellow, TEXT("Failed to spawn pawn here"));
		return;
	}
	
	for (const auto& SetupActionInstancedStruct : SpawnDescriptor.SetupPipeline)
	{
		if (!ensure(SetupActionInstancedStruct.IsValid()))
			continue;
		
		const auto& SetupAction = SetupActionInstancedStruct.Get<FSetupAction>();
		auto Memory = EpisodeSetupActionMemories.Contains(SetupAction.PipelineActionId) 
			? EpisodeSetupActionMemories[SetupAction.PipelineActionId].Get() 
			: nullptr;
		
		if (bRepeatSetup)
			SetupAction.Repeat(Pawn, Memory);
		else 
			SetupAction.Setup(Pawn, Memory);
	}
	
	SpawnedActors.Add(Pawn);
	
	CurrentSpawnIndex++;
	if (CurrentSpawnIndex == TrainingPresets[CurrentPresetIndex].CharactersSpawnDescriptors.Num())
		OnAllCharacterSpawned();	
}

void UTrainingEpisodeSetupComponent::FindEpisodeOriginLocation()
{
	TrainingPresets[CurrentPresetIndex].EpisodeOriginLocationEQS.Execute(*this, nullptr, FoundEpisodeOriginLocationDelegate);
}

void UTrainingEpisodeSetupComponent::StartSpawningCharacters()
{
	CurrentSpawnIndex = 0;
	StartSpawningNextCharacter();
}

void UTrainingEpisodeSetupComponent::StartSpawningNextCharacter()
{
	auto ActiveSpawnDescriptor = &TrainingPresets[CurrentPresetIndex].CharactersSpawnDescriptors[CurrentSpawnIndex];
	ActiveSpawnDescriptor->SpawnAgentLocationEQS.InitForOwnerAndBlackboard(*this, nullptr);
	ActiveSpawnDescriptor->InitialAgentLookAtEQS.InitForOwnerAndBlackboard(*this, nullptr);
	ActiveSpawnDescriptor->SpawnAgentLocationEQS.Execute(*this, nullptr, FoundSpawnLocationDelegate);
	ActiveSpawnDescriptor->InitialAgentLookAtEQS.Execute(*this, nullptr, FoundInitialLookAtLocationDelegate);
}

void UTrainingEpisodeSetupComponent::OnFoundEpisodeOriginLocation(TSharedPtr<FEnvQueryResult> EnvQueryResult)
{
	EpisodeOriginLocation = GetEQSLocation(EnvQueryResult);
	if (EpisodeOriginLocation == FVector::ZeroVector)
	{
		ensure(false);
		return;
	}

	if (PCGComponent.IsValid())
	{
		PCGComponent->Seed = EpisodeSeed;
		PCGComponent->Generate();
		bool bGenerating = PCGComponent->IsGenerating();
		bool bGenerating2 = PCGComponent->IsGenerationInProgress();
		ensure(bGenerating == true && bGenerating == bGenerating2);
	}
	else
	{
		StartSpawningCharacters();
	}
}

void UTrainingEpisodeSetupComponent::OnPCGCleanupCompleted(UPCGComponent* InPcgComponent)
{
	if (bSetupInProgress)
		FindEpisodeOriginLocation();
}

void UTrainingEpisodeSetupComponent::OnPCGGenerateCompleted(UPCGComponent* InPcgComponent)
{
	StartSpawningCharacters();
}

void UTrainingEpisodeSetupComponent::OnFoundSpawnLocation(TSharedPtr<FEnvQueryResult> Result)
{
	auto& SpawnDescriptor = TrainingPresets[CurrentPresetIndex].CharactersSpawnDescriptors[CurrentSpawnIndex];
	SpawnDescriptor.LastSpawnLocation = GetEQSLocation(Result);
	if (SpawnDescriptor.LastSpawnLocation != FVector::ZeroVector && SpawnDescriptor.LastInitialLookAtLocation != FVector::ZeroVector)
		SpawnCharacter(SpawnDescriptor, false);
}

void UTrainingEpisodeSetupComponent::OnFoundInitialLookAtLocation(TSharedPtr<FEnvQueryResult> EnvQueryResult)
{
	auto& SpawnDescriptor = TrainingPresets[CurrentPresetIndex].CharactersSpawnDescriptors[CurrentSpawnIndex];
	FVector LookAtLocation = GetEQSLocation(EnvQueryResult);
	if (LookAtLocation == FVector::ZeroVector)
	{
		LookAtLocation = FVector::ForwardVector;
		UE_VLOG(this, LogNpcMl, Warning, TEXT("Failed to get initial look at position for character"));
	}
	
	SpawnDescriptor.LastInitialLookAtLocation = LookAtLocation;
	if (SpawnDescriptor.LastSpawnLocation != FVector::ZeroVector && SpawnDescriptor.LastInitialLookAtLocation != FVector::ZeroVector)
		SpawnCharacter(SpawnDescriptor, false);
}

void UTrainingEpisodeSetupComponent::OnAllCharacterSpawned()
{
	TrainingEpisodeSetupCompletedEvent.Broadcast(TrainingPresets[CurrentPresetIndex]);
	bSetupInProgress = false;
}

FVector UTrainingEpisodeSetupComponent::GetEQSLocation(const TSharedPtr<FEnvQueryResult>& Result) const
{
	FVector Location = FVector::ZeroVector;
	if (!Result.IsValid() || Result->IsAborted())
	{
		UE_VLOG(this, LogNpcMl, Error, TEXT("Failed to get training episode setup location: 1"));
		return Location;
	}
	
	bool bSuccess = Result->IsSuccessful() && (Result->Items.Num() >= 1);
	if (!bSuccess)
	{
		UE_VLOG(this, LogNpcMl, Error, TEXT("Failed to get training episode setup location: 2"));
		return Location;
	}
	
	UEnvQueryItemType_VectorBase* ItemTypeCDO = Result->ItemType->GetDefaultObject<UEnvQueryItemType_VectorBase>();
	auto RawData = Result->RawData.GetData() + Result->Items[0].DataOffset;
	return ItemTypeCDO->GetItemLocation(RawData);
}
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
}

void UTrainingEpisodeSetupComponent::BeginPlay()
{
	Super::BeginPlay();
	FoundSpawnLocationDelegate.BindUObject(this, &UTrainingEpisodeSetupComponent::OnFoundSpawnLocation);
	FoundInitialLookAtLocationDelegate.BindUObject(this, &UTrainingEpisodeSetupComponent::OnFoundInitialLookAtLocation);
	FoundEpisodeOriginLocationDelegate.BindUObject(this, &UTrainingEpisodeSetupComponent::OnFoundEpisodeOriginLocation);
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
	
	for (auto& ActorSpawnDescriptor : TrainingPresets[CurrentPresetIndex].ActorsSpawnDescriptors)
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
	DestroySpawnedActors();
	for (const auto& SpawnDescriptor : TrainingPresets[CurrentPresetIndex].ActorsSpawnDescriptors)
		SpawnActor(SpawnDescriptor, true);
}

void UTrainingEpisodeSetupComponent::DestroySpawnedActors()
{
	for (const auto& SpawnedActor : SpawnedActors)
		SpawnedActor->Destroy();
	
	SpawnedActors.Empty();
	CurrentSpawnIndex = 0;
}

void UTrainingEpisodeSetupComponent::Cleanup()
{
	if (PCGComponent->IsGenerating())
		PCGComponent->StopGenerationInProgress();

	// TODO cancel active EQSs
	
	DestroySpawnedActors();
	EpisodeSetupActionMemories.Reset();
	if (PCGComponent.IsValid())
		PCGComponent->Cleanup();
}

void UTrainingEpisodeSetupComponent::SpawnActor(const FMLTrainingActorSpawnDescriptor& SpawnDescriptor, bool bRepeatSetup)
{
	FRotator Rotation = (SpawnDescriptor.LastInitialLookAtLocation - SpawnDescriptor.LastSpawnLocation).GetSafeNormal().Rotation();
	auto WorldLocal = GetWorld();
	FTransform SpawnTransform = FTransform(Rotation, SpawnDescriptor.LastSpawnLocation);
	auto SpawnClass = SpawnDescriptor.ActorClass.LoadSynchronous();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	auto Actor = SpawnDescriptor.bSpawnDeferred  
		? WorldLocal->SpawnActorDeferred<AActor>(SpawnClass, SpawnTransform, nullptr, nullptr, SpawnParameters.SpawnCollisionHandlingOverride)
		: WorldLocal->SpawnActor<AActor>(SpawnClass, SpawnDescriptor.LastSpawnLocation, Rotation, SpawnParameters);
	
	if (Actor == nullptr)
	{
		UE_VLOG(this, LogNpcMl, Warning, TEXT("Failed to spawn actor"));
		UE_VLOG_LOCATION(this, LogNpcMl, Warning, SpawnDescriptor.LastSpawnLocation, 25.f, FColor::Yellow, TEXT("Failed to spawn actor here"));
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
			SetupAction.Repeat(Actor, Memory);
		else 
			SetupAction.Setup(Actor, Memory);
	}
	
	SpawnedActors.Add(Actor);
	
	CurrentSpawnIndex++;
	if (CurrentSpawnIndex == TrainingPresets[CurrentPresetIndex].ActorsSpawnDescriptors.Num())
		OnAllActorsSpawned();	
}

void UTrainingEpisodeSetupComponent::FindEpisodeOriginLocation()
{
	TrainingPresets[CurrentPresetIndex].EpisodeOriginLocationEQS.Execute(*this, nullptr, FoundEpisodeOriginLocationDelegate);
}

void UTrainingEpisodeSetupComponent::StartSpawningActors()
{
	CurrentSpawnIndex = 0;
	StartSpawningNextActor();
}

void UTrainingEpisodeSetupComponent::StartSpawningNextActor()
{
	auto ActiveSpawnDescriptor = &TrainingPresets[CurrentPresetIndex].ActorsSpawnDescriptors[CurrentSpawnIndex];
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
		StartSpawningActors();
	}
}

void UTrainingEpisodeSetupComponent::OnPCGCleanupCompleted(UPCGComponent* InPcgComponent)
{
	if (bSetupInProgress)
		FindEpisodeOriginLocation();
}

void UTrainingEpisodeSetupComponent::OnPCGGenerateCompleted(UPCGComponent* InPcgComponent)
{
	StartSpawningActors();
}

void UTrainingEpisodeSetupComponent::OnFoundSpawnLocation(TSharedPtr<FEnvQueryResult> Result)
{
	auto& SpawnDescriptor = TrainingPresets[CurrentPresetIndex].ActorsSpawnDescriptors[CurrentSpawnIndex];
	SpawnDescriptor.LastSpawnLocation = GetEQSLocation(Result);
	if (SpawnDescriptor.LastSpawnLocation != FVector::ZeroVector && SpawnDescriptor.LastInitialLookAtLocation != FVector::ZeroVector)
		SpawnActor(SpawnDescriptor, false);
}

void UTrainingEpisodeSetupComponent::OnFoundInitialLookAtLocation(TSharedPtr<FEnvQueryResult> EnvQueryResult)
{
	auto& SpawnDescriptor = TrainingPresets[CurrentPresetIndex].ActorsSpawnDescriptors[CurrentSpawnIndex];
	FVector LookAtLocation = GetEQSLocation(EnvQueryResult);
	if (LookAtLocation == FVector::ZeroVector)
	{
		LookAtLocation = FVector::ForwardVector;
		UE_VLOG(this, LogNpcMl, Warning, TEXT("Failed to get initial look at position for actor"));
	}
	
	SpawnDescriptor.LastInitialLookAtLocation = LookAtLocation;
	if (SpawnDescriptor.LastSpawnLocation != FVector::ZeroVector && SpawnDescriptor.LastInitialLookAtLocation != FVector::ZeroVector)
		SpawnActor(SpawnDescriptor, false);
}

void UTrainingEpisodeSetupComponent::OnAllActorsSpawned()
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
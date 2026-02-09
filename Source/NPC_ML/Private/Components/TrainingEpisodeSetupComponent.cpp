#include "Components/TrainingEpisodeSetupComponent.h"

#include "NavigationSystem.h"
#include "PCGComponent.h"
#include "PCGGraph.h"
#include "AI/NavigationSystemBase.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Data/LogChannels.h"
#include "Data/MLTrainingPresetsDataAsset.h"
#include "Data/TrainingDataTypes.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "Interfaces/MLTrainingManagerPcgOwnerInterface.h"
#include "Subsystems/PCGSubsystem.h"

void UTrainingEpisodeSetupComponent::SetPCGComponent(UPCGComponent* InPCGComponent)
{
	if (InPCGComponent->GetGraph() == nullptr)
		return;
	
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
	if (bLockNavmeshForPcgUpdate && !ensure(FMath::IsPowerOfTwo(NavigationBuildLockFlag)))
	{
		UE_VLOG_UELOG(GetOwner(), LogNpcMl, Error, TEXT("Navigation build lock flag is not a power of two. UNavigationSystemV1 expects it to be a power of two"));
	}
}

void UTrainingEpisodeSetupComponent::SetupEpisode()
{
	if (!IsValid(TrainingPresetsDataAsset))
	{
		UE_VLOG_UELOG(GetOwner(), LogNpcMl, Error, TEXT("Training manager %s: no training presets data asset set. Cannot setup episode"), *GetOwner()->GetName())
		return;
	}
	
	if (bSetupInProgress)
	{
		UE_VLOG_UELOG(GetOwner(), LogNpcMl, Warning, TEXT("Training manager %s: episode setup is already in progress. Wait until setup finishes"), *GetOwner()->GetName())
		return;
	}
	
	bSetupInProgress = true;
	Cleanup();
	if (TrainingPresetsDataAsset->bRandomizeSeedOnSetup)
		EpisodeSeed = TrainingPresetsDataAsset->bRandomizeSeedOnSetup ? FMath::Rand32() : TrainingPresetsDataAsset->EpisodeSeed;
	
	if (TrainingPresetsDataAsset->bUseRandomEpisodeSetup)
		CurrentPresetIndex = FMath::RandRange(0, TrainingPresetsDataAsset->TrainingPresets.Num() - 1);
	else 
		CurrentPresetIndex = (CurrentPresetIndex + 1) % TrainingPresetsDataAsset->TrainingPresets.Num();
	
	int SpawnsCount = TrainingPresetsDataAsset->TrainingPresets[CurrentPresetIndex].ActorsSpawnDescriptors.Num();
	SpawnLocationsEQSRequests.SetNumZeroed(SpawnsCount);
	SpawnLookAtEQSRequests.SetNumZeroed(SpawnsCount);
	SpawnedActorsLocationsCached.SetNumZeroed(SpawnsCount);
	
	PendingLookAtLocationEQSRequests.Reserve(SpawnsCount);
	EpisodeSetupActionMemories.Reserve(SpawnsCount);
	
	for (const auto& ActorSpawnDescriptor : TrainingPresetsDataAsset->TrainingPresets[CurrentPresetIndex].ActorsSpawnDescriptors)
	{
		for (const auto& SetupActionInstancedStruct : ActorSpawnDescriptor.SetupPipeline)
		{
			if (ensure(SetupActionInstancedStruct.IsValid()))
			{
				const auto& SetupAction = SetupActionInstancedStruct.Get<FSetupAction>();
				TUniquePtr<FExternalMemory> ExternalMemory = SetupAction.MakeMemory();
				if (ExternalMemory.IsValid())
					EpisodeSetupActionMemories.Add(SetupAction.PipelineActionId, MoveTemp(ExternalMemory));
			}
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
	for (const auto& SpawnDescriptor : TrainingPresetsDataAsset->TrainingPresets[CurrentPresetIndex].ActorsSpawnDescriptors)
		SpawnActor(SpawnDescriptor, true);
}

void UTrainingEpisodeSetupComponent::Stop()
{
	Cleanup();
	if (bLockNavmeshForPcgUpdate && bNavMeshUpdateLocked)
	{
		if (auto NavSys = UNavigationSystemV1::GetCurrent(this))
			NavSys->RemoveNavigationBuildLock(NavigationBuildLockFlag);
		
		bNavMeshUpdateLocked = false;
	}
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
#if WITH_EDITOR
	if (PCGComponent.IsValid() && PCGComponent->IsGenerating())
		PCGComponent->StopGenerationInProgress();
#endif
	
	DestroySpawnedActors();
	EpisodeSetupActionMemories.Reset();
	
	// TODO cancel active EQSs. Need to store all EQS requests id first...
	// auto EQSManager = UEnvQueryManager::GetCurrent(this);
	// if (IsValid(EQSManager))
	// {
	// }
	
	SpawnLocationsEQSRequests.Empty();
	SpawnLookAtEQSRequests.Empty();
	SpawnedActorsLocationsCached.Empty();
	PendingLookAtLocationEQSRequests.Empty();
	EpisodeOriginLocation = FVector::ZeroVector;
	if (PCGComponent.IsValid())
	{
		if (bLockNavmeshForPcgUpdate && !bNavMeshUpdateLocked)
		{
			if (auto NavSys = UNavigationSystemV1::GetCurrent(this))
			{
				NavSys->AddNavigationBuildLock(NavigationBuildLockFlag);
				bNavMeshUpdateLocked = true;
			}
		}
		
		PCGComponent->Cleanup();
	}
}

void UTrainingEpisodeSetupComponent::FindEpisodeOriginLocation()
{
	LocalEpisodeOriginEQS = TrainingPresetsDataAsset->TrainingPresets[CurrentPresetIndex].EpisodeOriginLocationEQS;
	LocalEpisodeOriginEQS.InitForOwnerAndBlackboard(*this, nullptr);
	LocalEpisodeOriginEQS.Execute(*this, nullptr, FoundEpisodeOriginLocationDelegate);
}

void UTrainingEpisodeSetupComponent::StartSpawningActors()
{
	CurrentSpawnIndex = 0;
	StartSpawningNextActor();
}

void UTrainingEpisodeSetupComponent::StartSpawningNextActor()
{
	const auto& ActiveSpawnDescriptor = TrainingPresetsDataAsset->TrainingPresets[CurrentPresetIndex].ActorsSpawnDescriptors[CurrentSpawnIndex];
	SpawnLocationsEQSRequests[CurrentSpawnIndex] = ActiveSpawnDescriptor.SpawnActorLocationEQS;
	SpawnLocationsEQSRequests[CurrentSpawnIndex].InitForOwnerAndBlackboard(*this, nullptr);
	const int32 EqsId =  SpawnLocationsEQSRequests[CurrentSpawnIndex].Execute(*this, nullptr, FoundSpawnLocationDelegate);
	if (EqsId == INDEX_NONE)
	{
		ensure(false);
		SpawnedActorsLocationsCached[CurrentSpawnIndex].Reset();
		UE_VLOG_UELOG(GetOwner(), LogNpcMl, Warning, TEXT("Failed to run EQS for spawning actor at index %d"), CurrentSpawnIndex);
		IncrementSpawnedActors();
	}
}

void UTrainingEpisodeSetupComponent::SpawnActor(const FMLTrainingActorSpawnDescriptor& SpawnDescriptor, bool bRepeatSetup)
{
	auto& SpawnData = SpawnedActorsLocationsCached[CurrentSpawnIndex];
	const bool bNeedsLookAtEQS = SpawnData.LookAt == FVector::ZeroVector;
	FRotator Rotation = bNeedsLookAtEQS
		? FVector::ForwardVector.Rotation() 
		: (SpawnData.LookAt - SpawnData.SpawnLocation).GetSafeNormal().Rotation();
	
	auto WorldLocal = GetWorld();
	FTransform SpawnTransform = FTransform(Rotation, SpawnData.SpawnLocation);
	auto SpawnClass = SpawnDescriptor.ActorClass.LoadSynchronous();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	UE_LOG(LogNpcMl, Log, TEXT("Trying to spawn actor"));
	auto Actor = SpawnDescriptor.bSpawnDeferred  
		? WorldLocal->SpawnActorDeferred<AActor>(SpawnClass, SpawnTransform, nullptr, nullptr, SpawnParameters.SpawnCollisionHandlingOverride)
		: WorldLocal->SpawnActor<AActor>(SpawnClass, SpawnData.SpawnLocation, Rotation, SpawnParameters);
	
	if (Actor == nullptr)
	{
		ensure(false);
		UE_VLOG_UELOG(GetOwner(), LogNpcMl, Warning, TEXT("Failed to spawn actor [%d]"), CurrentSpawnIndex);
		UE_VLOG_LOCATION(this, LogNpcMl, Warning, SpawnData.SpawnLocation, 25.f, FColor::Yellow, TEXT("Failed to spawn actor here"));
		IncrementSpawnedActors();
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

	if (bNeedsLookAtEQS)
	{
		SpawnLookAtEQSRequests[CurrentSpawnIndex] = SpawnDescriptor.InitialActorLookAtEQS;
		SpawnLookAtEQSRequests[CurrentSpawnIndex].InitForOwnerAndBlackboard(*Actor, nullptr);
		const int32 LookAtEQSRequestId = SpawnLookAtEQSRequests[CurrentSpawnIndex].Execute(*Actor, nullptr, FoundInitialLookAtLocationDelegate);
		if (LookAtEQSRequestId != INDEX_NONE)
			PendingLookAtLocationEQSRequests.Add(LookAtEQSRequestId, { Actor,  CurrentSpawnIndex });
	}
	
	SpawnedActors.Add(Actor);
	IncrementSpawnedActors();
}

void UTrainingEpisodeSetupComponent::IncrementSpawnedActors()
{
	CurrentSpawnIndex++;
	if (CurrentSpawnIndex == TrainingPresetsDataAsset->TrainingPresets[CurrentPresetIndex].ActorsSpawnDescriptors.Num())
		OnAllActorsSpawned();
	else 
		StartSpawningNextActor();
}

void UTrainingEpisodeSetupComponent::OnFoundEpisodeOriginLocation(TSharedPtr<FEnvQueryResult> EnvQueryResult)
{
	EpisodeOriginLocation = GetEQSLocation(EnvQueryResult);
	if (EpisodeOriginLocation == FVector::ZeroVector)
	{
		ensure(false);
		UE_VLOG_UELOG(GetOwner(), LogNpcMl, Error, TEXT("Failed to find episode origin location"));
		FinishSetup();
		return;
	}

	if (PCGComponent.IsValid())
	{
		PCGComponent->Seed = EpisodeSeed;
		auto GraphInstance = PCGComponent->GetGraphInstance();
		if (!ensure(GraphInstance))
		{
			StartSpawningActors();
			return;
		}
		
		GraphInstance->SetGraphParameter(TrainingPresetsDataAsset->EpisodeSeedParameterName, EpisodeSeed);
		
		for (const auto& FloatParameter : TrainingPresetsDataAsset->TrainingPresets[CurrentPresetIndex].PcgParameters.FloatParameters)
		{
			auto ParameterSetResult = GraphInstance->SetGraphParameter(FloatParameter.Key, FloatParameter.Value);
			ensure(ParameterSetResult == EPropertyBagResult::Success);
		}
		
		if (PCGComponent->GetOwner()->Implements<UMLTrainingManagerPcgOwnerInterface>())
			IMLTrainingManagerPcgOwnerInterface::Execute_SetTrainingEpisodeOrigin(PCGComponent->GetOwner(), EpisodeOriginLocation);
		
#if WITH_EDITOR
		// alas this works, but these functions are editor only
		// PCGComponent->ResetLastGeneratedBounds();
		// PCGComponent->DirtyGenerated(EPCGComponentDirtyFlag::All, true);
#endif
		
		// this shit doesn't seem to be working. release-ready-in-5.7-my ass
		// UPCGSubsystem::GetInstance(GetWorld())->FlushCache();
		
		if (bLockNavmeshForPcgUpdate && !bNavMeshUpdateLocked)
		{
			if (auto NavSys = UNavigationSystemV1::GetCurrent(this))
			{
				NavSys->AddNavigationBuildLock(NavigationBuildLockFlag);
				bNavMeshUpdateLocked = true;
			}
		}
		
		PCGComponent->Generate();
		const bool bGenerating = PCGComponent->IsGenerating();
		ensure(bGenerating);
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
	if (UNavigationSystemV1* NavSys = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem()))
	{
		if (DebugOptions.Contains(12))
		{
			TArray<TObjectPtr<UInstancedStaticMeshComponent>> SpawnedStaticMeshComponents; 
			// TArray<UActorComponent*> ISMCDummies;
			InPcgComponent->GetOwner()->GetComponents(SpawnedStaticMeshComponents);
			for (auto& ISMC : SpawnedStaticMeshComponents)
			{
				if (IsValid(ISMC))
				{
					if (DebugOptions.Contains(1))
						ISMC->SetCanEverAffectNavigation(true);
				
					if (DebugOptions.Contains(2))
					{
						ISMC->SetCanEverAffectNavigation(false);
						ISMC->SetCanEverAffectNavigation(true);
					}
				
					if (DebugOptions.Contains(3))
						ISMC->UpdateNavigationBounds();
				
					if (DebugOptions.Contains(4))
						NavSys->UpdateComponentInNavOctree(*ISMC);	
				}
			}
		}

		if (DebugOptions.Contains(5))
			NavSys->Build();
		
		if (bLockNavmeshForPcgUpdate && bNavMeshUpdateLocked)
		{
			NavSys->RemoveNavigationBuildLock(ENavigationBuildLock::Custom, UNavigationSystemV1::ELockRemovalRebuildAction::Rebuild);
			bNavMeshUpdateLocked = false;
		}
	}
	
	// Force rebuild of navmesh tiles
	// if (UNavigationSystemV1* NavSys = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem()))
	// 	NavSys->Build();
	
	StartSpawningActors();
}

void UTrainingEpisodeSetupComponent::OnFoundSpawnLocation(TSharedPtr<FEnvQueryResult> Result)
{
	auto& SpawnDescriptor = TrainingPresetsDataAsset->TrainingPresets[CurrentPresetIndex].ActorsSpawnDescriptors[CurrentSpawnIndex];
	const FVector SpawnLocation = GetEQSLocation(Result);
	if (SpawnLocation != FVector::ZeroVector)
	{
		SpawnedActorsLocationsCached[CurrentSpawnIndex].SpawnLocation = SpawnLocation;
		SpawnActor(SpawnDescriptor, false);
	}
	else
	{
		UE_VLOG_UELOG(GetOwner(), LogNpcMl, Warning, TEXT("Failed to spawn actor [%d]. EQS find spawn location failed"), CurrentSpawnIndex);
		IncrementSpawnedActors();
	}
}

void UTrainingEpisodeSetupComponent::OnFoundInitialLookAtLocation(TSharedPtr<FEnvQueryResult> EnvQueryResult)
{
	const auto& LookAtSetupData = PendingLookAtLocationEQSRequests.Find(EnvQueryResult->QueryID);
	if (LookAtSetupData == nullptr)
		return;
	
	FVector LookAtLocation = GetEQSLocation(EnvQueryResult);
	if (LookAtLocation == FVector::ZeroVector)
	{
		LookAtLocation = FVector::ForwardVector;
		PendingLookAtLocationEQSRequests.Remove(EnvQueryResult->QueryID);
		UE_VLOG(GetOwner(), LogNpcMl, Warning, TEXT("Failed to get an initial look at position for actor"));
		return;
	}
	
	SpawnedActorsLocationsCached[LookAtSetupData->SpawnDescriptorIndex].LookAt = LookAtLocation;
	LookAtSetupData->Actor->SetActorRotation((LookAtLocation - LookAtSetupData->Actor->GetActorLocation()).GetSafeNormal().Rotation());
	PendingLookAtLocationEQSRequests.Remove(EnvQueryResult->QueryID);
}

void UTrainingEpisodeSetupComponent::FinishSetup()
{
	TrainingEpisodeSetupCompletedEvent.Broadcast(TrainingPresetsDataAsset->TrainingPresets[CurrentPresetIndex]);
	bSetupInProgress = false;
}

void UTrainingEpisodeSetupComponent::OnAllActorsSpawned()
{
	FinishSetup();
}

FVector UTrainingEpisodeSetupComponent::GetEQSLocation(const TSharedPtr<FEnvQueryResult>& Result) const
{
	FVector Location = FVector::ZeroVector;
	if (!Result.IsValid() || Result->IsAborted())
	{
		UE_VLOG(GetOwner(), LogNpcMl, Error, TEXT("Failed to get EQS location: 1"));
		return Location;
	}
	
	bool bSuccess = Result->IsSuccessful() && (Result->Items.Num() >= 1);
	if (!bSuccess)
	{
		UE_VLOG(GetOwner(), LogNpcMl, Error, TEXT("Failed to get EQS location: 2"));
		return Location;
	}
	
	UEnvQueryItemType_VectorBase* ItemTypeCDO = Result->ItemType->GetDefaultObject<UEnvQueryItemType_VectorBase>();
	auto RawData = Result->RawData.GetData() + Result->Items[0].DataOffset;
	return ItemTypeCDO->GetItemLocation(RawData);
}
// 

#pragma once

#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "Components/ActorComponent.h"
#include "Data/TrainingDataTypes.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "TrainingEpisodeSetupComponent.generated.h"


class ATrainingEpisodePCG;
class UMLTrainingPresetsDataAsset;
struct FMLTrainingEpisodeActorSetupAction_Base;
struct FMLTrainingActorSpawnDescriptor;
struct FTrainingEpisodeSetupActionExternalMemoryBase;
struct FMLTrainingPreset;
class UPCGComponent;
class UEnvQuery;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NPC_ML_API UTrainingEpisodeSetupComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	DECLARE_MULTICAST_DELEGATE_OneParam(FTrainingEpisodeSetupCompletedEvent, const FMLTrainingPreset& Preset);
	DECLARE_MULTICAST_DELEGATE_OneParam(FTrainingEpisodeSetupStepChangedEvent, ETrainingEpisodeSetupAction CurrentStep);
	
	using FExternalMemory = FTrainingEpisodeSetupActionExternalMemoryBase;
	using FSetupAction = FMLTrainingEpisodeActorSetupAction_Base;
	using FSetupPipeline = TArray<TInstancedStruct<FSetupAction>>;
	
	struct FPendingLookAtEQSData
	{
		TWeakObjectPtr<AActor> Actor;
		int SpawnDescriptorIndex = -1;
	};
	
	struct FSpawnedActorsLocations
	{
		FSpawnedActorsLocations() {  }
		
		FVector SpawnLocation = FVector::ZeroVector; 
		FVector LookAt = FVector::ZeroVector;
		
		void Reset() { SpawnLocation = FVector::ZeroVector; LookAt = FVector::ZeroVector; };
		bool IsValid() const { return SpawnLocation != FVector::ZeroVector && LookAt != FVector::ZeroVector; };
	};
	
public:
	/* Pipeline:
	 * 1. Cleanup previous PCG generated objects
	 * 2. Run EQS for episode origin location
	 * 3. Parametrize and run PCG generator
	 * 4. Run EQS again for agent spawn location
	 * 5. Spawn agent
	 * 6. Spawn NPCs
	 */
	virtual void SetupEpisode();

	virtual void RepeatEpisode();
	virtual void Stop();
	
	FORCEINLINE const FVector& GetEpisodeOriginLocation() const { return EpisodeOriginLocation; };

	FTrainingEpisodeSetupCompletedEvent TrainingEpisodeSetupCompletedEvent;
	FTrainingEpisodeSetupStepChangedEvent TrainingEpisodeSetupStepChangedEvent;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	const UMLTrainingPresetsDataAsset* TrainingPresetsDataAsset;

	// 9 Feb 2026 (aki)
	// just a convenient array of number to opt in and out certain code blocks. 
	// none of the numbers have any specific meaning aside from the code blocks they enable
	// TODO remove ASAP when PCG setup works correctly
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<int> DebugOptions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bLockNavmeshForPcgUpdate = true;

	// Must be a power of 2
	// 9 Feb 2026 (aki): 
	// TODO think about getting it from a not existing yet game mode interface "uint8 INpcMlGameMode::GetNavigationBuildPcgLockFlag()"
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMIn = 16, ClampMin = 16))
	uint8 NavigationBuildLockFlag = ENavigationBuildLock::Custom;
	
	void SpawnNewActor(const FMLTrainingActorSpawnDescriptor& SpawnDescriptor);
	void RespawnActor(const FMLTrainingActorSpawnDescriptor& SpawnDescriptor);
	
	virtual void OnPCGCleanupCompleted();
	virtual void OnPCGGenerateCompleted();
	virtual void Cleanup();

private:
	int CurrentPresetIndex = 0;
	int CurrentSpawnIndex = 0;
	
	TMap<FGuid, FExternalMemoryPtr> EpisodeSetupActionMemories;
	TMap<FGuid, TWeakObjectPtr<AActor>> SpawnedActors;
	
	FEQSParametrizedQueryExecutionRequest LocalEpisodeOriginEQS;

	TArray<FEQSParametrizedQueryExecutionRequest> SpawnLocationsEQSRequests;
	TArray<FEQSParametrizedQueryExecutionRequest> SpawnLookAtEQSRequests;
	
	TArray<FSpawnedActorsLocations> SpawnedActorsLocationsCached;
	
	// key - EQS request id
	TMap<int32, FPendingLookAtEQSData> PendingLookAtLocationEQSRequests;
	
	FQueryFinishedSignature FoundEpisodeOriginLocationDelegate;
	FQueryFinishedSignature FoundSpawnLocationDelegate;
	FQueryFinishedSignature FoundInitialLookAtLocationDelegate;

	FVector EpisodeOriginLocation = FVector::ZeroVector;
	int32 EpisodeSeed = 0;
	ETrainingEpisodeSetupAction PendingStepPostNavmeshRegenerate = ETrainingEpisodeSetupAction::None;
	bool bSetupInProgress = false;
	bool bRepeatingEpisode = false;
	bool bNavMeshUpdateLocked = false;
	
	UNavigationSystemV1::ELockRemovalRebuildAction NavMeshLockRemovalAction = UNavigationSystemV1::ELockRemovalRebuildAction::NoRebuild;

	FVector GetEQSLocation(const TSharedPtr<FEnvQueryResult>& Result) const;
	
	UPROPERTY()
	TObjectPtr<ATrainingEpisodePCG> TrainingEpisodePCG;
	
	void DestroySpawnedActors();
	void InitializeEpisodeActorSetupActionsMemories(const FSetupPipeline& Pipeline);
	void FindEpisodeOriginLocation();
	void StartSpawningNextActor();
	AActor* SpawnActor(const FMLTrainingActorSpawnDescriptor& SpawnDescriptor);
	void IncrementSpawnedActors();
	void StartSpawningActors();
	void OnFoundEpisodeOriginLocation(TSharedPtr<FEnvQueryResult> EnvQueryResult);
	void OnFoundSpawnLocation(TSharedPtr<FEnvQueryResult> EnvQueryResult);
	void OnFoundInitialLookAtLocation(TSharedPtr<FEnvQueryResult> EnvQueryResult);
	void ExecuteActorOnStartActions();
	void ExecuteActorSetupPipeline(AActor* Actor, const FSetupPipeline& SetupPipeline);
	void FinishSetup();
	void OnAllActorsSpawned();
	void CleanTrainingEpisodePCG();
	
	void SpawnTrainingEpisodePCG(const FMLTrainingEpisodePCG& EpisodePCG);
	
	void LockNavMeshUpdate();
	void UnlockNavmeshUpdate();
	bool WaitNavmeshUpdate(ETrainingEpisodeSetupAction ScheduledActionPostNavmeshUpdate);
	
	UFUNCTION()
	void OnNavMeshRegenerated(ANavigationData* NavData);
};

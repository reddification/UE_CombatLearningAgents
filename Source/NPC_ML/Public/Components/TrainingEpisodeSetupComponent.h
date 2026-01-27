// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "TrainingEpisodeSetupComponent.generated.h"


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
	
	using FExternalMemory = FTrainingEpisodeSetupActionExternalMemoryBase;
	using FSetupAction = FMLTrainingEpisodeActorSetupAction_Base;
	
public:
	 // Runtime generation must be enabled on the PCG component / project settings appropriate to your setup;
	 // otherwise you’ll see “works in editor, not in packaged” symptoms.
	void SetPCGComponent(UPCGComponent* InPCGComponent);
		
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
	virtual void Cleanup();
	
	FORCEINLINE const FVector& GetEpisodeOriginLocation() const { return EpisodeOriginLocation; };

	FTrainingEpisodeSetupCompletedEvent TrainingEpisodeSetupCompletedEvent;

	// update EQS config params and whatever else there is going to be
#if WITH_EDITOR
	UFUNCTION(BlueprintCallable, CallInEditor)
	void UpdateEditorParams();
#endif
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FMLTrainingPreset> TrainingPresets;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUseRandomEpisodeSetup = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bRandomizeSeedOnSetup = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EpisodeSeed = 12345;
	
	// x = random from 0 to 1, y = density of static meshes in proximity of agent
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRuntimeFloatCurve PCGGeometryDensityDistribution;

	// if spawning is deferred, OnAgentSpawned will not be called. caller must finalize the spawn on their own
	virtual void SpawnActor(FMLTrainingActorSpawnDescriptor& SpawnDescriptor, bool bRepeatSetup);
	
	virtual void OnPCGCleanupCompleted(UPCGComponent* InPcgComponent);
	virtual void OnPCGGenerateCompleted(UPCGComponent* InPcgComponent);

private:
	TWeakObjectPtr<UPCGComponent> PCGComponent;
	int CurrentPresetIndex = 0;
	int CurrentSpawnIndex = 0;
	
	TMap<FGuid, TUniquePtr<FExternalMemory>> EpisodeSetupActionMemories;
	TArray<TWeakObjectPtr<AActor>> SpawnedActors;
	
	struct FPendingLookAtEQSData
	{
		TWeakObjectPtr<AActor> Actor;
		int SpawnDescriptorIndex = -1;
	};
	
	TMap<int32, FPendingLookAtEQSData> PendingEQSLookAtLocationSetups;
	
	FQueryFinishedSignature FoundEpisodeOriginLocationDelegate;
	FQueryFinishedSignature FoundSpawnLocationDelegate;
	FQueryFinishedSignature FoundInitialLookAtLocationDelegate;

	FVector EpisodeOriginLocation = FVector::ZeroVector;
	bool bSetupInProgress = false;
	
	FVector GetEQSLocation(const TSharedPtr<FEnvQueryResult>& Result) const;
	
	void DestroySpawnedActors();
	void FindEpisodeOriginLocation();
	void StartSpawningNextActor();
	void IncrementSpawnedActors();
	void StartSpawningActors();
	void OnFoundEpisodeOriginLocation(TSharedPtr<FEnvQueryResult> EnvQueryResult);
	void OnFoundSpawnLocation(TSharedPtr<FEnvQueryResult> EnvQueryResult);
	void OnFoundInitialLookAtLocation(TSharedPtr<FEnvQueryResult> EnvQueryResult);
	void OnAllActorsSpawned();
};

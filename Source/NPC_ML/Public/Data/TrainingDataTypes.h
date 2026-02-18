#pragma once

#include "TrainingEpisodeActorSetupActions.h"
#include "EnvironmentQuery/EnvQueryTypes.h"

#include "StructUtils/InstancedStruct.h"
#include "TrainingDataTypes.generated.h"

class ATrainingEpisodePCG;
class UPCGGraph;

UENUM()
enum class EMLTrainingSessionState
{
	Inactive,
	Running,
	Paused,
	SettingUpEpisode,
	PreparingToStart,
	Restarting,
};

UENUM()
enum class ETrainingEpisodeSetupAction : uint8
{
	None,
	PCG_Cleanup,
	PCG_Generate,
	WaitForNavmeshToGenerate,
	FindEpisodeOriginLocation,
	SpawnActors
};

USTRUCT(BlueprintType)
struct FPcgParametersContainer
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, float> FloatParameters;
};

UCLASS(BlueprintType)
class UMLTrainingEpisodeActorTemplate : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly)
	FString UserDescription;
#endif
	
	UPROPERTY(EditAnywhere)
	FEQSParametrizedQueryExecutionRequest SpawnActorLocationEQS;
	
	UPROPERTY(EditAnywhere)
	FEQSParametrizedQueryExecutionRequest InitialActorLookAtEQS;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bSpawnDeferred = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<AActor> ActorClass;
	
	// these actions will be applied immediately on spawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FMLTrainingEpisodeActorSetupAction_Base>> SetupPipelineOnSpawn;
	
	// these actions will be applied when all actors have already spawned and right before the training episode starts
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FMLTrainingEpisodeActorSetupAction_Base>> SetupPipelineOnStart;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};

USTRUCT(BlueprintType)
struct FMLTrainingActorSpawnDescriptor
{
	GENERATED_BODY()

	FMLTrainingActorSpawnDescriptor()
	{
		Id = FGuid::NewGuid();
	}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGuid Id;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMLTrainingEpisodeActorTemplate* Template = nullptr;

	// episode specific. will be applied after template actions 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FMLTrainingEpisodeActorSetupAction_Base>> SetupPipelineOnSpawn;
	
	// episode specific. will be applied after template actions
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FMLTrainingEpisodeActorSetupAction_Base>> SetupPipelineOnStart;
};

USTRUCT(BlueprintType)
struct FMLTrainingEpisodePCG
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UPCGGraph> PCG;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<ATrainingEpisodePCG> PCGContainerClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FPcgParametersContainer PcgParameters;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bOverrideExtents = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="bOverrideExtents"))
	FVector Extents = FVector(20000.f, 20000.f, 20000.f);
	
	bool IsValid() const { return !PCG.IsNull() && !PCGContainerClass.IsNull(); };
};

USTRUCT(BlueprintType)
struct FMLTrainingPreset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ConfigTitle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ConfigDescription;
	
	//  currently only matters for PCG
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float EpisodeAreaRadius = 2500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DurationMin = 15.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DurationMax = 45.f;
	
	UPROPERTY(EditAnywhere)
	FEQSParametrizedQueryExecutionRequest EpisodeOriginLocationEQS;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMLTrainingEpisodePCG EpisodePCG;
	
	// both agent and dummy-NPCs go here
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FMLTrainingActorSpawnDescriptor> ActorsSpawnDescriptors;
};


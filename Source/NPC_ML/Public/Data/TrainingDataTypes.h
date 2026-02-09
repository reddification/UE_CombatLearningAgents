#pragma once
#include "EnvironmentQuery/EnvQueryTypes.h"

#include "TrainingDataTypes.generated.h"

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

struct FTrainingEpisodeSetupActionExternalMemoryBase
{
	bool bCanSetup = true;
};

using FExternalMemory = FTrainingEpisodeSetupActionExternalMemoryBase;

struct FTrainingEpisodeSetupExternalMemory_Spawn : public FExternalMemory
{
	FVector SpawnLocation = FVector::ZeroVector;
	FVector LookAt = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FPcgParametersContainer
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, float> FloatParameters;
};

USTRUCT(BlueprintType)
struct NPC_ML_API FMLTrainingEpisodeActorSetupAction_Base
{
	GENERATED_BODY()
	
	using Super = FMLTrainingEpisodeActorSetupAction_Base;

	FMLTrainingEpisodeActorSetupAction_Base()
	{
		PipelineActionId = FGuid::NewGuid();
	};
	
	virtual ~FMLTrainingEpisodeActorSetupAction_Base() = default;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGuid PipelineActionId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(UIMin = 0.f, ClampMin = 0.f, UIMax = 1.f, ClampMax = 1.f))
	float SetupChance = 1.f;
	
	bool Setup(AActor* Actor, FExternalMemory* Memory) const
	{
		if (CanSetup(Actor))
		{
			if (Memory != nullptr)
				Memory->bCanSetup = true;
				
			return SetupInternal(Actor, Memory);
		}
		
		return false;
	};
	
	bool Repeat(AActor* Actor, FExternalMemory* Memory) const
	{
		if (Memory == nullptr || Memory->bCanSetup)
			return RepeatInternal(Actor, Memory);
		
		return false;
	};
	
	virtual float CanSetup(AActor* Actor) const
	{
		return SetupChance >= 1.f ? true : FMath::RandRange(0.f, 1.f) <= SetupChance;
	}
	
	virtual TUniquePtr<FExternalMemory> MakeMemory() const { return MakeUnique<FExternalMemory>(); }
	
protected:
	virtual bool SetupInternal(AActor* Actor, FExternalMemory* Memory) const { return true; }
	virtual bool RepeatInternal(AActor* Actor, FExternalMemory* Memory) const { return true; }
};

USTRUCT(BlueprintType, DisplayName="Finish deferred spawn")
struct FMLTrainingEpisodeActorSetupAction_FinishDeferredSpawn : public FMLTrainingEpisodeActorSetupAction_Base
{
	GENERATED_BODY()
	
protected:
	virtual bool SetupInternal(AActor* Actor, FExternalMemory* Memory) const override;
};

USTRUCT(BlueprintType)
struct FMLTrainingActorSpawnDescriptor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FEQSParametrizedQueryExecutionRequest SpawnActorLocationEQS;
	
	UPROPERTY(EditAnywhere)
	FEQSParametrizedQueryExecutionRequest InitialActorLookAtEQS;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bSpawnDeferred = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<AActor> ActorClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FMLTrainingEpisodeActorSetupAction_Base>> SetupPipeline;
};

USTRUCT(BlueprintType)
struct FMLTrainingPreset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ConfigTitle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ConfigDescription;
	
	UPROPERTY(EditAnywhere)
	FEQSParametrizedQueryExecutionRequest EpisodeOriginLocationEQS;
	
	//  currently only matters for PCG
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float EpisodeAreaRadius = 2500.f;
	
	// both agent and dummy-NPCs go here
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FMLTrainingActorSpawnDescriptor> ActorsSpawnDescriptors;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DurationMin = 15.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DurationMax = 45.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPcgParametersContainer PcgParameters;
};


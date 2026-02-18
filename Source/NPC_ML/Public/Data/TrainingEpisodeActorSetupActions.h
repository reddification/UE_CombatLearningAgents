#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "TrainingEpisodeActorSetupActions.generated.h"

struct FTrainingEpisodeSetupActionExternalMemoryBase
{
	virtual ~FTrainingEpisodeSetupActionExternalMemoryBase() = default;
	bool bCanSetup = true;
};

using FExternalMemory = FTrainingEpisodeSetupActionExternalMemoryBase;
using FExternalMemoryPtr = TSharedPtr<FExternalMemory>;

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActive = true;
	
	bool Setup(AActor* Actor, FExternalMemoryPtr Memory) const;
	bool Repeat(AActor* Actor, FExternalMemoryPtr Memory) const;
	virtual float CanSetup(AActor* Actor) const;
	
	virtual FExternalMemoryPtr MakeMemory() const { return MakeShared<FExternalMemory>(); }
	
protected:
	virtual bool SetupInternal(AActor* Actor, FExternalMemoryPtr Memory) const { return true; }
	virtual bool RepeatInternal(AActor* Actor, FExternalMemoryPtr Memory) const { return SetupInternal(Actor, Memory); }
};

USTRUCT(BlueprintType, DisplayName="Finish deferred spawn")
struct FMLTrainingEpisodeActorSetupAction_FinishDeferredSpawn : public FMLTrainingEpisodeActorSetupAction_Base
{
	GENERATED_BODY()
	
protected:
	virtual bool SetupInternal(AActor* Actor, FExternalMemoryPtr Memory) const override;
};
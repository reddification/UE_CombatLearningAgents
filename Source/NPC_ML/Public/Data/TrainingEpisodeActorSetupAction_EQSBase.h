#pragma once

#include "CoreMinimal.h"
#include "TrainingEpisodeActorSetupActions.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "TrainingEpisodeActorSetupAction_EQSBase.generated.h"

USTRUCT(BlueprintType)
struct FMLSetupActionEQSParameter
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Name;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Value;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EAIParamType Type;
};

struct FTrainingEpisodeSetupActionExternalMemory_EQS : public FExternalMemory
{
	bool Execute(const FEQSParametrizedQueryExecutionRequest& Request, AActor* InOwner, const TArray<FMLSetupActionEQSParameter>& EQSParams);
		
	TWeakObjectPtr<AActor> Owner;
	TWeakObjectPtr<AActor> ResultActor;
	FVector ResultLocation = FVector::ZeroVector;
	bool bEQSSucceeded = false;
	FQueryFinishedSignature QueryFinishedDelegate;

	void OnEQSQueryCompleted(TSharedPtr<FEnvQueryResult> EnvQueryResult);

protected:
	virtual void OnEQSQueryLocationResult(const FVector& Location) { ResultLocation = Location; }
	virtual void OnEQSQueryActorResult(AActor* Actor) { ResultActor = Actor; ResultLocation = Actor->GetActorLocation(); };
	
private:
	FEQSParametrizedQueryExecutionRequest Request;
};

using FExternalMemory_EQS = FTrainingEpisodeSetupActionExternalMemory_EQS;

USTRUCT(DisplayName="Run EQS", meta=(Hidden))
struct NPC_ML_API FCharacterInitializationSetupPipeline_RunEQS_Base : public FMLTrainingEpisodeActorSetupAction_Base
{
	GENERATED_BODY()

public:
	virtual FExternalMemoryPtr MakeMemory() const override;
	
protected:
	virtual bool SetupInternal(AActor* Actor, FExternalMemoryPtr ExternalMemory) const override;
	
	UPROPERTY(EditAnywhere)
	FEQSParametrizedQueryExecutionRequest EQSRequest;

	UPROPERTY(EditAnywhere)
	TArray<FMLSetupActionEQSParameter> EQSParams;	
};

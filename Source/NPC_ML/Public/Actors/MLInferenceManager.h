#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsPolicy.h"
#include "MLManagerBase.h"
#include "MLInferenceManager.generated.h"

class UMLModelVersion;
class ULearningAgentsPolicy;
class ULearningAgentsInteractor;

UCLASS()
class NPC_ML_API AMLInferenceManager : public AMLManagerBase
{
	GENERATED_BODY()

public:
	AMLInferenceManager();
	virtual void SetManagerActive(bool bNewActive) override;
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UMLModelVersion> Model;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Seed = 1234;

private:
	UPROPERTY()
	TObjectPtr<ULearningAgentsPolicy> Policy;
};

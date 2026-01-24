// 

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsPolicy.h"
#include "MLManagerBase.h"
#include "MLInferenceManager.generated.h"

class ULearningAgentsNeuralNetwork;
class ULearningAgentsPolicy;
class ULearningAgentsInteractor;

UCLASS()
class NPC_ML_API AMLInferenceManager : public AMLManagerBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMLInferenceManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsInteractor> InteractorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsPolicy> PolicyClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ULearningAgentsNeuralNetwork* EncoderNN;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ULearningAgentsNeuralNetwork* PolicyNN;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ULearningAgentsNeuralNetwork* DecoderNN;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsPolicySettings PolicySettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Seed = 1234;

private:
	UPROPERTY()
	TObjectPtr<ULearningAgentsInteractor> Interactor;

	UPROPERTY()
	TObjectPtr<ULearningAgentsPolicy> Policy;
};

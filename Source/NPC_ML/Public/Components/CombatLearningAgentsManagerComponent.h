// 

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsCommunicator.h"
#include "LearningAgentsCritic.h"
#include "LearningAgentsManager.h"
#include "LearningAgentsNeuralNetwork.h"
#include "LearningAgentsPolicy.h"
#include "LearningAgentsPPOTrainer.h"
#include "LearningAgentsTrainer.h"
#include "CombatLearningAgentsManagerComponent.generated.h"


class ULearningAgentsPPOTrainer;
class ULearningAgentsCritic;
class ULearningAgentsPolicy;
class ULearningAgentsTrainingEnvironment;
class ULearningAgentsInteractor;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NPC_ML_API UCombatLearningAgentsManagerComponent : public ULearningAgentsManager
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCombatLearningAgentsManagerComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
								   FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsInteractor> InteractorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsTrainingEnvironment> TrainingEnvironmentClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsPolicy> PolicyClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsCritic> CriticClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsPPOTrainer> PPOTrainerClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bRunInference = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ULearningAgentsNeuralNetwork* EncoderNN;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ULearningAgentsNeuralNetwork* PolicyNN;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ULearningAgentsNeuralNetwork* DecoderNN;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ULearningAgentsNeuralNetwork* CriticNN;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bReinitializeWeights = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsPolicySettings PolicySettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Seed = 1234;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsCriticSettings CriticSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsTrainerProcessSettings TrainerProcessSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsSharedMemoryCommunicatorSettings SharedMemorySettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsPPOTrainerSettings PPOTrainerSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsPPOTrainingSettings TrainerTrainingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsTrainingGameSettings TrainingGameSettings;


private:
	UPROPERTY()
	TObjectPtr<ULearningAgentsInteractor> Interactor;

	UPROPERTY()
	TObjectPtr<ULearningAgentsTrainingEnvironment> TrainingEnvironment;

	UPROPERTY()
	TObjectPtr<ULearningAgentsPolicy> Policy;

	UPROPERTY()
	TObjectPtr<ULearningAgentsCritic> Critic;

	UPROPERTY()
	TObjectPtr<ULearningAgentsPPOTrainer> PPOTrainer;
	
	FLearningAgentsSharedMemoryTrainerProcess SharedMemory;
	FLearningAgentsCommunicator Communicator;
};

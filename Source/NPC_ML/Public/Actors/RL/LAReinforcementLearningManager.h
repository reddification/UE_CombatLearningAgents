// 

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsCommunicator.h"
#include "LearningAgentsCritic.h"
#include "LearningAgentsPolicy.h"
#include "LearningAgentsPPOTrainer.h"
#include "LearningAgentsTrainer.h"
#include "Actors/MLTrainingManager.h"
#include "GameFramework/Actor.h"
#include "LAReinforcementLearningManager.generated.h"

class ULearningAgentsNeuralNetwork;
class ULearningAgentsPPOTrainer;
class ULearningAgentsCritic;
class ULearningAgentsPolicy;
class ULearningAgentsTrainingEnvironment;
class ULearningAgentsInteractor;
class ULearningAgentsManager;
class ULAReinforcementLearningManagerComponent;

UCLASS()
class NPC_ML_API ALAReinforcementLearningManager : public AMLTrainingManager
{
	GENERATED_BODY()

public:
	ALAReinforcementLearningManager();

protected:
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsTrainingEnvironment> TrainingEnvironmentClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsPolicy> PolicyClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsCritic> CriticClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsPPOTrainer> PPOTrainerClass;
	
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

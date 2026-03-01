// 

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsCommunicator.h"
#include "LearningAgentsCritic.h"
#include "LearningAgentsPolicy.h"
#include "LearningAgentsPPOTrainer.h"
#include "Actors/MLTrainingManager.h"
#include "Data/MLTrainingConfigurationBase.h"
#include "GameFramework/Actor.h"
#include "LAReinforcementLearningManager.generated.h"

class ULearningAgentsPPOTrainer;
class ULearningAgentsCritic;
class ULearningAgentsPolicy;
class ULearningAgentsTrainingEnvironment;

UCLASS()
class NPC_ML_API ALAReinforcementLearningManager : public AMLTrainingManager
{
	GENERATED_BODY()

public:
	ALAReinforcementLearningManager();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<ULearningAgentsTrainingEnvironment> TrainingEnvironment;
 
	UPROPERTY()
	TObjectPtr<ULearningAgentsPolicy> Policy;

	UPROPERTY()
	TObjectPtr<ULearningAgentsCritic> Critic;

	UPROPERTY()
	TObjectPtr<ULearningAgentsPPOTrainer> PPOTrainer;

	UPROPERTY()
	TObjectPtr<UMLTrainingConfiguration_RL> RLConfiguration;
	
	FLearningAgentsSharedMemoryTrainerProcess SharedMemory;
	FLearningAgentsCommunicator Communicator;
};

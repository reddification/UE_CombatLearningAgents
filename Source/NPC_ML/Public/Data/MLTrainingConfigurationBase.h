// 

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsCommunicator.h"
#include "LearningAgentsImitationTrainer.h"
#include "LearningAgentsPPOTrainer.h"
#include "LearningAgentsRecorder.h"
#include "LearningAgentsTrainer.h"
#include "Engine/DataAsset.h"
#include "MLTrainingConfigurationBase.generated.h"

class ULearningAgentsCombatController;
class UMLModelVersion;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NPC_ML_API UMLTrainingConfigurationBase : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
	FString ConfigurationName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Info")
	FString UserDescription;	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UMLModelVersion> ModelVersion;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Seed = 1234;
};

UCLASS(Abstract, Blueprintable)
class NPC_ML_API UMLTrainingConfiguration : public UMLTrainingConfigurationBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsTrainerProcessSettings TrainerProcessSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsSharedMemoryCommunicatorSettings SharedMemorySettings;

};

UCLASS(Blueprintable)
class NPC_ML_API UMLTrainingConfiguration_IL : public UMLTrainingConfiguration
{
	GENERATED_BODY()
	
public:
	UMLTrainingConfiguration_IL();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsImitationTrainer> ImitationTrainerClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsImitationTrainerSettings TrainerSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsImitationTrainerTrainingSettings TrainerTrainingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bDoLivePreview = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InferenceTickRate = 0.1f;
};

UCLASS(Blueprintable)
class NPC_ML_API UMLTrainingConfiguration_RL : public UMLTrainingConfiguration
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsPPOTrainerSettings PPOTrainerSettings;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsPPOTrainingSettings TrainerTrainingSettings;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsTrainingGameSettings TrainingGameSettings;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsTrainingEnvironment> TrainingEnvironmentClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsPPOTrainer> PPOTrainerClass;
};

UCLASS(Blueprintable)
class NPC_ML_API UMLTrainingConfiguration_IR : public UMLTrainingConfigurationBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsCombatController> ILControllerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsRecorder> ILRecorderClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsRecorderPathSettings RecorderPathSettings;
};


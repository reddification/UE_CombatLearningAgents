// 

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsCommunicator.h"
#include "LearningAgentsImitationTrainer.h"
#include "LearningAgentsPolicy.h"
#include "Actors/MLManagerBase.h"
#include "Actors/MLTrainingManager.h"
#include "GameFramework/Actor.h"
#include "ImitationLearningTrainingManager.generated.h"

class ULearningAgentsNeuralNetwork;
class ULearningAgentsPolicy;
class ULearningAgentsInteractor;
class ULearningAgentsManager;

UCLASS()
class NPC_ML_API AImitationLearningTrainingManager : public AMLTrainingManager
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AImitationLearningTrainingManager();
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsPolicy> PolicyClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ULearningAgentsNeuralNetwork* EncoderNN;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ULearningAgentsNeuralNetwork* PolicyNN;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ULearningAgentsNeuralNetwork* DecoderNN;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bReinitializeWeights = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsPolicySettings PolicySettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Seed = 1234;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsTrainerProcessSettings TrainerProcessSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsSharedMemoryCommunicatorSettings SharedMemorySettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsImitationTrainer> ImitationTrainerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ULearningAgentsImitationTrainer* ImitationTrainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	const ULearningAgentsRecording* RecordingAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsImitationTrainerSettings ImitationTrainerSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsImitationTrainerTrainingSettings ImitationTrainerTrainingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsTrainerProcessSettings ImitationTrainerPathSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AccumulationTimeInterval = 100.f;
	
private:
	UPROPERTY()
	TObjectPtr<ULearningAgentsPolicy> Policy;

	FLearningAgentsSharedMemoryTrainerProcess SharedMemory;
	FLearningAgentsCommunicator Communicator;

	float AccumulatedTime = 0.f;

	void ResetTestingGameEnvironment();
};

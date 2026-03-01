#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsCommunicator.h"
#include "LearningAgentsImitationTrainer.h"
#include "LearningAgentsPolicy.h"
#include "Actors/MLTrainingManager.h"
#include "Data/MLTrainingConfigurationBase.h"
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
	
	virtual void OnTrainingFinished();
	
private:
	UPROPERTY()
	TObjectPtr<ULearningAgentsPolicy> Policy;

	UPROPERTY()
	TObjectPtr<ULearningAgentsImitationTrainer> ImitationTrainer;
	
	UPROPERTY()
	TObjectPtr<UMLTrainingConfiguration_IL> ILConfiguration = nullptr;

	UPROPERTY()
	TObjectPtr<ULearningAgentsRecording> RecordingAsset = nullptr;
	
	FLearningAgentsSharedMemoryTrainerProcess SharedMemory;
	FLearningAgentsCommunicator Communicator;
	
	float DelayTillNextInference = 0.f;
	
};

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsController.h"
#include "LearningAgentsRecorder.h"
#include "GameFramework/Actor.h"
#include "LearningAgentsImitationCombatRecordingManager.generated.h"

class ULearningAgentsInteractor;
class ULearningAgentsManager;

UCLASS()
class NPC_ML_API ALearningAgentsImitationCombatRecordingManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALearningAgentsImitationCombatRecordingManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ULearningAgentsManager* LearningAgentsManager;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsInteractor> InteractorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsController> ILControllerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsRecorder> ILRecorderClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsRecorderPathSettings RecorderPathSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ULearningAgentsRecording* RecordingAsset;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LearningTime = 120.f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	TObjectPtr<ULearningAgentsInteractor> Interactor;

	UPROPERTY()
	TObjectPtr<ULearningAgentsController> ILController;

	UPROPERTY()
	TObjectPtr<ULearningAgentsRecorder> ILRecorder;

	float AccumulatedTime = 0.f;

	void ResetLearningAgents();
};

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LearningAgentsController.h"
#include "LearningAgentsRecorder.h"
#include "Data/LearningAgentsDataTypes.h"
#include "GameFramework/Actor.h"
#include "LearningAgentsImitationCombatRecordingManager.generated.h"

class ULearningAgentsCombatController;

namespace LearningAgentsImitationActions
{
	struct FAgentPendingActions;
}

class ULearningAgentsInteractor;
class ULearningAgentsManager;

UCLASS()
class NPC_ML_API ALearningAgentsImitationCombatRecordingManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALearningAgentsImitationCombatRecordingManager();

private:
	struct FAgentPendingAction
	{
		FVector MoveDirection;
		FRotator Rotation;
		float MoveSpeed = 0.f;
		
		FVector JumpDirection;
		FVector ClimbDirection;
		int AttackAction = -1;
	};
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ULearningAgentsManager* LearningAgentsManager;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsInteractor> InteractorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsCombatController> ILControllerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsRecorder> ILRecorderClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLearningAgentsRecorderPathSettings RecorderPathSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ULearningAgentsRecording* RecordingAsset;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LearningTime = 120.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RecordInterval = 0.1f;

public:
	void RegisterMove(AActor* Agent, const FVector& Direction);
	void RegisterMoveSpeed(AActor* Agent, float MoveSpeed);
	void RegisterRotate(AActor* Agent, const FRotator& NewRotator);
	void RegisterJump(AActor* Agent);
	void RegisterMantle(AActor* Agent);
	void RegisterAttack(AActor* Agent, int AttackIndex);
	void RegisterParry(AActor* Agent, const FVector& ParryDirection);
	void RegisterDodge(AActor* Agent, const FVector& DodgeDirection);
	void RegisterGesture(AActor* Agent, const FGameplayTag& GestureTag);
	void RegisterPhrase(AActor* Agent, const FGameplayTag& PhraseTag);
	void RegisterUseConsumableItem(AActor* Agent, const FGameplayTag& ItemId);
	void RegisterWeaponStateChange(AActor* Agent, ELAWeaponStateChange NewState);

protected:
	virtual void ResetLearningAgents();
	
private:
	UPROPERTY()
	TObjectPtr<ULearningAgentsInteractor> Interactor;

	UPROPERTY()
	TObjectPtr<ULearningAgentsCombatController> ILController;

	UPROPERTY()
	TObjectPtr<ULearningAgentsRecorder> ILRecorder;
	
	FTimerHandle RecordTimer;
	FTimerHandle RestartTimer;

	void RecordImitations();
	void RestartLearning();
	
	LearningAgentsImitationActions::FAgentPendingActions& GetAgentActionsQueue(AActor* Agent);
	LearningAgentsImitationActions::FAgentPendingActions& GetAgentActionsQueue(int AgentId);
	
	void OnActionAccumulated(int AgentId);
};

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LearningAgentsRecorder.h"
#include "Actors/MLTrainingManager.h"
#include "Data/LearningAgentsDataTypes.h"
#include "GameFramework/Actor.h"

#include "ImitationLearningRecordingManager.generated.h"

class UMLOverviewPanelWidget;
enum class EMLTrainingSessionState;
class UPCGComponent;
class UTrainingEpisodeSetupComponent;
class ULearningAgentsCombatController;

namespace LearningAgentsImitationActions
{
	struct FAgentPendingActionsBuffer;
}

class ULearningAgentsInteractor;
class ULearningAgentsManager;

UCLASS()
class NPC_ML_API AImitationLearningRecordingManager : public AMLTrainingManager
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AImitationLearningRecordingManager();
	virtual void Tick(float DeltaSeconds) override;
	
	void RegisterMove(AActor* Agent, const FVector& WorldDirection);
	void RegisterMoveSpeed(AActor* Agent, float MoveSpeed);
	void RegisterRotate(AActor* Agent, const FRotator& NewRotator);
	void RegisterJump(AActor* Agent);
	void RegisterMantle(AActor* Agent);
	void RegisterAttack(AActor* Agent, uint8 AttackType, UEnum* AttackEnum);
	void RegisterParry(AActor* Agent);
	void RegisterDodge(AActor* Agent, const FVector& DodgeDirectionWorld);
	void RegisterGesture(AActor* Agent, const FGameplayTag& GestureTag);
	void RegisterPhrase(AActor* Agent, const FGameplayTag& PhraseTag);
	void RegisterUseConsumableItem(AActor* Agent, const FGameplayTag& ItemId);
	void RegisterWeaponStateChange(AActor* Agent, ELAWeaponStateChange NewState);

	virtual void ResumeTraining() override;
	virtual void PauseTraining() override;
	virtual void RestartTraining(bool bUseNewSetup) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY()
	TObjectPtr<ULearningAgentsCombatController> ILController;

	UPROPERTY()
	TObjectPtr<ULearningAgentsRecorder> ILRecorder;
	
	LearningAgentsImitationActions::FAgentPendingActionsBuffer& GetAgentActionsQueue(AActor* Agent);
	LearningAgentsImitationActions::FAgentPendingActionsBuffer& GetAgentActionsQueue(int AgentId);

	void RecordImitations();
	void OnActionAccumulated(int AgentId);
};

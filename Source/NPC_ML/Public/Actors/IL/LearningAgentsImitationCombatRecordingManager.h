#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LearningAgentsController.h"
#include "LearningAgentsRecorder.h"
#include "Data/LearningAgentsDataTypes.h"
#include "Data/TrainingDataTypes.h"
#include "GameFramework/Actor.h"

#include "LearningAgentsImitationCombatRecordingManager.generated.h"

class UMLOverviewPanelWidget;
enum class EMLTrainingSessionState;
class UPCGComponent;
class UTrainingEpisodeSetupComponent;
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

private:
	DECLARE_MULTICAST_DELEGATE_OneParam(FTrainingEpisodeStateChangedEvent, EMLTrainingSessionState);
	
public:
	// Sets default values for this actor's properties
	ALearningAgentsImitationCombatRecordingManager();

	void RegisterMove(AActor* Agent, const FVector& WorldDirection);
	void RegisterMoveSpeed(AActor* Agent, float MoveSpeed);
	void RegisterRotate(AActor* Agent, const FRotator& NewRotator);
	void RegisterJump(AActor* Agent);
	void RegisterMantle(AActor* Agent);
	void RegisterAttack(AActor* Agent, uint8 AttackType, UEnum* AttackEnum);
	void RegisterParry(AActor* Agent, float ParryAngle);
	void RegisterDodge(AActor* Agent, const FVector& DodgeDirectionWorld);
	void RegisterGesture(AActor* Agent, const FGameplayTag& GestureTag);
	void RegisterPhrase(AActor* Agent, const FGameplayTag& PhraseTag);
	void RegisterUseConsumableItem(AActor* Agent, const FGameplayTag& ItemId);
	void RegisterWeaponStateChange(AActor* Agent, ELAWeaponStateChange NewState);

	void StartImitationRecording();
	void StartNextImitationLearning();
	void ResumeImitationRecording();
	void PauseImitationRecording();
	void StopImitationRecording();
	void RestartImitationRecording(bool bUseNewSetup);
	
	FTrainingEpisodeStateChangedEvent TrainingEpisodeStateChangedEvent;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void OnEpisodeSetupCompleted(const FMLTrainingPreset& TrainingPreset);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ULearningAgentsManager* LearningAgentsManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UTrainingEpisodeSetupComponent> TrainingEpisodeSetupComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPCGComponent> PCGComponent;
	
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = 0.001f, ClampMin = 0.001f))
	float RecordInterval = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = 0.001f, ClampMin = 0.001f))
	float StartRecordingDelay = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAddDebugPanelWidget = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<UMLOverviewPanelWidget> DebugPanelWidgetClass;
	
	EMLTrainingSessionState TrainingState = EMLTrainingSessionState::Inactive;
	TWeakObjectPtr<UMLOverviewPanelWidget> DebugPanelWidget;

private:
	UPROPERTY()
	TObjectPtr<ULearningAgentsInteractor> Interactor;

	UPROPERTY()
	TObjectPtr<ULearningAgentsCombatController> ILController;

	UPROPERTY()
	TObjectPtr<ULearningAgentsRecorder> ILRecorder;
	
	FTimerHandle RecordTimer;
	FTimerHandle RestartTimer;
	FTimerHandle StartRecordingDelayTimer;
	float LearningTime = 0.f;
	
	void SetState(EMLTrainingSessionState NewState);
	void RecordImitations();
	
	LearningAgentsImitationActions::FAgentPendingActions& GetAgentActionsQueue(AActor* Agent);
	LearningAgentsImitationActions::FAgentPendingActions& GetAgentActionsQueue(int AgentId);
	
	void OnActionAccumulated(int AgentId);
};

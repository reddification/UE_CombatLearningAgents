#include "Actors/IL/ImitationLearningRecordingManager.h"

#include "LearningAgentsController.h"
#include "LearningAgentsInteractor.h"
#include "LearningAgentsManager.h"
#include "LearningAgentsRecorder.h"
#include "LearningAgentsRecording.h"
#include "Controllers/LearningAgentsCombatController.h"
#include "Data/LearningAgentsTags_Combat.h"
#include "Data/LogChannels.h"
#include "Data/MLModelVersion.h"
#include "Data/MLTrainingConfigurationBase.h"
#include "Data/ILActions/ILAction_Attack.h"
#include "Data/ILActions/ILAction_Base.h"
#include "Data/ILActions/ILAction_Parry.h"
#include "Data/ILActions/ILAction_ChangeWeaponState.h"
#include "Data/ILActions/ILAction_Dodge.h"
#include "Data/ILActions/ILAction_Gesture.h"
#include "Data/ILActions/ILAction_Locomotion_BlockingAction.h"
#include "Data/ILActions/ILAction_Locomotion_Move.h"
#include "Data/ILActions/ILAction_Locomotion_Rotate.h"
#include "Data/ILActions/ILAction_Locomotion_SetSpeed.h"
#include "Data/ILActions/ILAction_Phrase.h"
#include "Data/ILActions/ILAction_UseConsumableItem.h"
#include "LearningEntities/Interactors/LearningAgentsInteractor_Base.h"

using namespace LearningAgentsImitationActions;

AImitationLearningRecordingManager::AImitationLearningRecordingManager()
{
	BehaviorTag = LearningAgentsTags_Combat::Behavior_Combat_IL_Recording;
}

void AImitationLearningRecordingManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	RecordImitations();
}

// Called when the game starts or when spawned
void AImitationLearningRecordingManager::BeginPlay()
{
	Super::BeginPlay();

	auto IRConfiguration = Cast<UMLTrainingConfiguration_IR>(TrainingConfiguration);
	if (!IRConfiguration || IRConfiguration->ModelVersion.IsNull())
		{ ensure(false); return; }
	
	auto ModelVersion = IRConfiguration->ModelVersion.LoadSynchronous();
	if (ModelVersion->RecordingAsset.IsNull())
		{ ensure(false); return; }
	
	auto RecordingAsset = ModelVersion->RecordingAsset.LoadSynchronous();
	auto ManagerPtr = LearningAgentsManager.Get();
	ULearningAgentsInteractor* InteractorPtr = Interactor.Get();
	ILController = Cast<ULearningAgentsCombatController>(ULearningAgentsController::MakeController(ManagerPtr, InteractorPtr, IRConfiguration->ILControllerClass));
	bool bReinitializeRecording = true;
	ILRecorder = ULearningAgentsRecorder::MakeRecorder(ManagerPtr, InteractorPtr, IRConfiguration->ILRecorderClass, FName("IL Recorder"),
		IRConfiguration->RecorderPathSettings, RecordingAsset, bReinitializeRecording);
	Interactor->SetImitationRecordingModeActive(true);
}

void AImitationLearningRecordingManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (auto World = GetWorld())
	{
		auto& TimerManager = World->GetTimerManager();
		TimerManager.ClearAllTimersForObject(this);
	}
	
	if (ILRecorder != nullptr)
		ILRecorder->EndRecording();
	
	Super::EndPlay(EndPlayReason);
}

void AImitationLearningRecordingManager::ResumeTraining()
{
	Super::ResumeTraining();
	ILRecorder->BeginRecording();
}

void AImitationLearningRecordingManager::PauseTraining()
{
	ILRecorder->EndRecording();
	Super::PauseTraining();
}

void AImitationLearningRecordingManager::RestartTraining(bool bUseNewSetup)
{
	ILRecorder->EndRecording();
	Super::RestartTraining(bUseNewSetup);
	// TODO (aki) 22.01.2026: consider implementing doing a rollback of recorded data
}

FAgentPendingActionsBuffer& AImitationLearningRecordingManager::GetAgentActionsQueue(AActor* Agent)
{
	auto AgentId = LearningAgentsManager->GetAgentId(Agent);
	return GetAgentActionsQueue(AgentId);
}

FAgentPendingActionsBuffer& AImitationLearningRecordingManager::GetAgentActionsQueue(int AgentId)
{
	auto& PendingActionQueues = ILController->GetPendingActionBuffer(); 
	if (!PendingActionQueues.Contains(AgentId))
	{
		PendingActionQueues.Add(AgentId, FAgentPendingActionsBuffer(AgentId));
		PendingActionQueues[AgentId].ActionAccumulatedEvent.BindUObject(this, &AImitationLearningRecordingManager::OnActionAccumulated);
	}
	
	return PendingActionQueues[AgentId];
}

void AImitationLearningRecordingManager::RecordImitations()
{
	ILController->RunController();
	ILRecorder->AddExperience();
}

void AImitationLearningRecordingManager::RegisterMove(AActor* Agent, const FVector& WorldDirection)
{
	FAgentPendingActionsBuffer& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Locomotion_Move> NewAction = MakeShared<FAction_Locomotion_Move>(GetWorld()->GetTimeSeconds(), WorldDirection);
	Queue.AddAction(NewAction);
}

void AImitationLearningRecordingManager::RegisterMoveSpeed(AActor* Agent, float MoveSpeed)
{
	FAgentPendingActionsBuffer& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Locomotion_SetSpeed> NewAction = MakeShared<FAction_Locomotion_SetSpeed>(GetWorld()->GetTimeSeconds(), MoveSpeed);
	Queue.AddAction(NewAction);
}

void AImitationLearningRecordingManager::RegisterRotate(AActor* Agent, const FRotator& NewRotator)
{
	FAgentPendingActionsBuffer& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Locomotion_Rotate> NewAction = MakeShared<FAction_Locomotion_Rotate>(GetWorld()->GetTimeSeconds(), NewRotator);
	Queue.AddAction(NewAction);
}

void AImitationLearningRecordingManager::RegisterJump(AActor* Agent)
{
	FAgentPendingActionsBuffer& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Locomotion_BlockingLocomotion> NewAction = MakeShared<FAction_Locomotion_BlockingLocomotion>(GetWorld()->GetTimeSeconds(), ELALocomotionAction::Jump);
	Queue.AddAction(NewAction);
}

void AImitationLearningRecordingManager::RegisterMantle(AActor* Agent)
{
	FAgentPendingActionsBuffer& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Locomotion_BlockingLocomotion> NewAction = MakeShared<FAction_Locomotion_BlockingLocomotion>(GetWorld()->GetTimeSeconds(), ELALocomotionAction::Mantle);
	Queue.AddAction(NewAction);
}

void AImitationLearningRecordingManager::RegisterAttack(AActor* Agent, uint8 AttackType, UEnum* AttackEnum)
{
	FAgentPendingActionsBuffer& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Attack> NewAction = MakeShared<FAction_Attack>(GetWorld()->GetTimeSeconds(), AttackType, AttackEnum);
	Queue.AddAction(NewAction);
}

void AImitationLearningRecordingManager::RegisterParry(AActor* Agent)
{
	FAgentPendingActionsBuffer& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Parry> NewAction = MakeShared<FAction_Parry>(GetWorld()->GetTimeSeconds());
	Queue.AddAction(NewAction);
}

void AImitationLearningRecordingManager::RegisterDodge(AActor* Agent, const FVector& DodgeDirectionWorld)
{
	FAgentPendingActionsBuffer& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Dodge> NewAction = MakeShared<FAction_Dodge>(GetWorld()->GetTimeSeconds(), DodgeDirectionWorld);
	Queue.AddAction(NewAction);
}

void AImitationLearningRecordingManager::RegisterGesture(AActor* Agent, const FGameplayTag& GestureTag)
{
	FAgentPendingActionsBuffer& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Gesture> NewAction = MakeShared<FAction_Gesture>(GetWorld()->GetTimeSeconds(), GestureTag);
	Queue.AddAction(NewAction);
}

void AImitationLearningRecordingManager::RegisterPhrase(AActor* Agent, const FGameplayTag& PhraseTag)
{
	FAgentPendingActionsBuffer& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Phrase> NewAction = MakeShared<FAction_Phrase>(GetWorld()->GetTimeSeconds(), PhraseTag);
	Queue.AddAction(NewAction);
}

void AImitationLearningRecordingManager::RegisterUseConsumableItem(AActor* Agent, const FGameplayTag& ItemId)
{
	FAgentPendingActionsBuffer& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_UseConsumableItem> NewAction = MakeShared<FAction_UseConsumableItem>(GetWorld()->GetTimeSeconds(), ItemId);
	Queue.AddAction(NewAction);
}

void AImitationLearningRecordingManager::RegisterWeaponStateChange(AActor* Agent,
	ELAWeaponStateChange NewState)
{
	FAgentPendingActionsBuffer& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_ChangeWeaponState> NewAction = MakeShared<FAction_ChangeWeaponState>(GetWorld()->GetTimeSeconds(), NewState);
	Queue.AddAction(NewAction);
}

void AImitationLearningRecordingManager::OnActionAccumulated(int AgentId)
{
	int AgentsNum = LearningAgentsManager->GetAgentNum();
	if (AgentsNum > 1)
	{
		// TODO find a way to do it per agent
		UE_VLOG(this, LogNpcMl, Warning, TEXT("IL: re-running agents observations and actions for all agents because of 1 agent %d"), AgentId);
	}
	
	// I'm not sure if it's a good idea to record imitations right after an imitator makes an action
	// wouldn't it break "stride" or "temporal regularity of input" of training?
	// TODO (aki) 24.01.2026 consider
	// 23 Feb 2026 (aki): in theory, it won't, because ILController->RunController() gathers new observations
	// and ILRecorder->AddExperience() gets new action for active observations. 
	RecordImitations();
}


// 


#include "Actors/IL/LearningAgentsImitationCombatRecordingManager.h"

#include "LearningAgentsController.h"
#include "LearningAgentsInteractor.h"
#include "LearningAgentsManager.h"
#include "LearningAgentsRecorder.h"
#include "Controllers/LearningAgentsCombatController.h"
#include "Data/ImitationLearningDataTypes.h"
#include "Data/LogChannels.h"
#include "Subsystems/LearningAgentSubsystem.h"

using namespace LearningAgentsImitationActions;

// Sets default values
ALearningAgentsImitationCombatRecordingManager::ALearningAgentsImitationCombatRecordingManager()
{
	PrimaryActorTick.bCanEverTick = false;
	LearningAgentsManager = CreateDefaultSubobject<ULearningAgentsManager>(TEXT("LearningAgentsManager"));
}

// Called when the game starts or when spawned
void ALearningAgentsImitationCombatRecordingManager::BeginPlay()
{
	Super::BeginPlay();
	auto LAS = GetWorld()->GetSubsystem<ULearningAgentSubsystem>();
	LAS->RegisterLearningAgentsManager(LearningAgentsManager);
	LAS->RegisterImitationLearningRecordingManager(this);

	Interactor = ULearningAgentsInteractor::MakeInteractor(LearningAgentsManager, InteractorClass, FName("RecorderCombatInteractor"));
	auto InteractorPtr = Interactor.Get();

	ILController = Cast<ULearningAgentsCombatController>(ULearningAgentsController::MakeController(LearningAgentsManager, InteractorPtr, ILControllerClass));
	bool bReinitializeRecording = true;
	ILRecorder = ULearningAgentsRecorder::MakeRecorder(LearningAgentsManager, InteractorPtr, ILRecorderClass, FName("ILCombatRecorder"),
	                                                   RecorderPathSettings, RecordingAsset, bReinitializeRecording);

	ResetLearningAgents();
	ILRecorder->BeginRecording();
	
	auto& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(RecordTimer, this, &ALearningAgentsImitationCombatRecordingManager::RecordImitations, RecordInterval);
	TimerManager.SetTimer(RestartTimer, this, &ALearningAgentsImitationCombatRecordingManager::RestartLearning, LearningTime);
}

void ALearningAgentsImitationCombatRecordingManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (auto World = GetWorld())
	{
		if (auto LAS = World->GetSubsystem<ULearningAgentSubsystem>())
		{
			LAS->UnregisterLearningAgentsManager(LearningAgentsManager);
			LAS->UnregisterImitationLearningRecordingManager(this);
		}
	}
	
	if (ILRecorder != nullptr)
		ILRecorder->EndRecording();
	
	Super::EndPlay(EndPlayReason);
}

void ALearningAgentsImitationCombatRecordingManager::RecordImitations()
{
	ILController->RunController();
	ILRecorder->AddExperience();
}

void ALearningAgentsImitationCombatRecordingManager::RestartLearning()
{
	ILRecorder->EndRecording();
	LearningAgentsManager->ResetAllAgents();
	ResetLearningAgents();
	ILRecorder->BeginRecording();
}

FAgentPendingActions& ALearningAgentsImitationCombatRecordingManager::GetAgentActionsQueue(AActor* Agent)
{
	auto AgentId = LearningAgentsManager->GetAgentId(Agent);
	return GetAgentActionsQueue(AgentId);
}

FAgentPendingActions& ALearningAgentsImitationCombatRecordingManager::GetAgentActionsQueue(int AgentId)
{
	auto& PendingActionQueues = ILController->GetPendingActionQueues(); 
	if (!PendingActionQueues.Contains(AgentId))
	{
		PendingActionQueues.Add(AgentId, FAgentPendingActions(AgentId));
		PendingActionQueues[AgentId].ActionAccumulatedEvent.BindUObject(this, &ALearningAgentsImitationCombatRecordingManager::OnActionAccumulated);
	}
	
	return PendingActionQueues[AgentId];
}

void ALearningAgentsImitationCombatRecordingManager::RegisterMove(AActor* Agent, const FVector& Direction)
{
	FAgentPendingActions& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Locomotion_Move> NewAction = MakeShared<FAction_Locomotion_Move>(GetWorld()->GetTimeSeconds(), Direction);
	Queue.AddAction(NewAction);
}

void ALearningAgentsImitationCombatRecordingManager::RegisterMoveSpeed(AActor* Agent, float MoveSpeed)
{
	FAgentPendingActions& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Locomotion_SetSpeed> NewAction = MakeShared<FAction_Locomotion_SetSpeed>(GetWorld()->GetTimeSeconds(), MoveSpeed);
	Queue.AddAction(NewAction);
}

void ALearningAgentsImitationCombatRecordingManager::RegisterRotate(AActor* Agent, const FRotator& NewRotator)
{
	FAgentPendingActions& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Locomotion_Rotate> NewAction = MakeShared<FAction_Locomotion_Rotate>(GetWorld()->GetTimeSeconds(), NewRotator);
	Queue.AddAction(NewAction);
}

void ALearningAgentsImitationCombatRecordingManager::RegisterJump(AActor* Agent)
{
	FAgentPendingActions& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Locomotion_BlockingLocomotion> NewAction = MakeShared<FAction_Locomotion_BlockingLocomotion>(GetWorld()->GetTimeSeconds(), ELALocomotionAction::Jump);
	Queue.AddAction(NewAction);
}

void ALearningAgentsImitationCombatRecordingManager::RegisterMantle(AActor* Agent)
{
	FAgentPendingActions& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Locomotion_BlockingLocomotion> NewAction = MakeShared<FAction_Locomotion_BlockingLocomotion>(GetWorld()->GetTimeSeconds(), ELALocomotionAction::Mantle);
	Queue.AddAction(NewAction);
}

void ALearningAgentsImitationCombatRecordingManager::RegisterAttack(AActor* Agent, int AttackIndex)
{
	FAgentPendingActions& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Attack> NewAction = MakeShared<FAction_Attack>(GetWorld()->GetTimeSeconds(), AttackIndex);
	Queue.AddAction(NewAction);
}

void ALearningAgentsImitationCombatRecordingManager::RegisterParry(AActor* Agent, const FVector& ParryDirection)
{
	FAgentPendingActions& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Parry> NewAction = MakeShared<FAction_Parry>(GetWorld()->GetTimeSeconds(), ParryDirection);
	Queue.AddAction(NewAction);
}

void ALearningAgentsImitationCombatRecordingManager::RegisterDodge(AActor* Agent, const FVector& DodgeDirection)
{
	FAgentPendingActions& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Dodge> NewAction = MakeShared<FAction_Dodge>(GetWorld()->GetTimeSeconds(), DodgeDirection);
	Queue.AddAction(NewAction);
}

void ALearningAgentsImitationCombatRecordingManager::RegisterGesture(AActor* Agent, const FGameplayTag& GestureTag)
{
	FAgentPendingActions& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Locomotion_Gesture> NewAction = MakeShared<FAction_Locomotion_Gesture>(GetWorld()->GetTimeSeconds(), GestureTag);
	Queue.AddAction(NewAction);
}

void ALearningAgentsImitationCombatRecordingManager::RegisterPhrase(AActor* Agent, const FGameplayTag& PhraseTag)
{
	FAgentPendingActions& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Locomotion_Phrase> NewAction = MakeShared<FAction_Locomotion_Phrase>(GetWorld()->GetTimeSeconds(), PhraseTag);
	Queue.AddAction(NewAction);
}

void ALearningAgentsImitationCombatRecordingManager::RegisterUseConsumableItem(AActor* Agent, const FGameplayTag& ItemId)
{
	FAgentPendingActions& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Locomotion_UseConsumableItem> NewAction = MakeShared<FAction_Locomotion_UseConsumableItem>(GetWorld()->GetTimeSeconds(), ItemId);
	Queue.AddAction(NewAction);
}

void ALearningAgentsImitationCombatRecordingManager::RegisterWeaponStateChange(AActor* Agent,
	ELAWeaponStateChange NewState)
{
	FAgentPendingActions& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_ChangeWeaponState> NewAction = MakeShared<FAction_ChangeWeaponState>(GetWorld()->GetTimeSeconds(), NewState);
	Queue.AddAction(NewAction);
}

void ALearningAgentsImitationCombatRecordingManager::ResetLearningAgents()
{
	unimplemented();
}

void ALearningAgentsImitationCombatRecordingManager::OnActionAccumulated(int AgentId)
{
	int AgentsNum = LearningAgentsManager->GetAgentNum();
	if (AgentsNum > 1)
	{
		// TODO find a way to do it per agent
		UE_VLOG(this, LogNpcMl, Warning, TEXT("IL: re-running agents observations and actions for all agents because of 1 agent %d"), AgentId);
	}
	
	RecordImitations();
}


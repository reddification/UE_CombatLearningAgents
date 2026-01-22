// 


#include "Actors/IL/LearningAgentsImitationCombatRecordingManager.h"

#include "LearningAgentsController.h"
#include "LearningAgentsInteractor.h"
#include "LearningAgentsManager.h"
#include "LearningAgentsRecorder.h"
#include "PCGComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/TrainingEpisodeSetupComponent.h"
#include "Controllers/LearningAgentsCombatController.h"
#include "Data/ImitationLearningDataTypes.h"
#include "Data/LogChannels.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/LearningAgentSubsystem.h"
#include "UI/MLOverviewPanelWidget.h"

using namespace LearningAgentsImitationActions;

ALearningAgentsImitationCombatRecordingManager::ALearningAgentsImitationCombatRecordingManager()
{
	PrimaryActorTick.bCanEverTick = false;
	LearningAgentsManager = CreateDefaultSubobject<ULearningAgentsManager>(TEXT("LearningAgentsManager"));
	TrainingEpisodeSetupComponent = CreateDefaultSubobject<UTrainingEpisodeSetupComponent>(TEXT("Training Episodes Setup Component"));
	PCGComponent = CreateDefaultSubobject<UPCGComponent>(TEXT("PCG Component"));
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
	
	if (bAddDebugPanelWidget && ensure(!DebugPanelWidgetClass.IsNull()))
	{
		auto WidgetClass = DebugPanelWidgetClass.LoadSynchronous();
		auto PC = UGameplayStatics::GetPlayerController(this, 0);
		DebugPanelWidget = CreateWidget<UMLOverviewPanelWidget>(PC, WidgetClass);
		DebugPanelWidget->AddToViewport();
	}
}

void ALearningAgentsImitationCombatRecordingManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	TrainingEpisodeSetupComponent->SetPCGComponent(PCGComponent);
	TrainingEpisodeSetupComponent->TrainingEpisodeSetupCompletedEvent.AddUObject(this, &ALearningAgentsImitationCombatRecordingManager::OnEpisodeSetupCompleted);
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
		
		auto& TimerManager = World->GetTimerManager();
		TimerManager.ClearAllTimersForObject(this);
	}
	
	if (ILRecorder != nullptr)
		ILRecorder->EndRecording();
	
	Super::EndPlay(EndPlayReason);
}

void ALearningAgentsImitationCombatRecordingManager::OnEpisodeSetupCompleted(const FMLTrainingPreset& TrainingPreset)
{
	auto& TimerManager = GetWorld()->GetTimerManager();
	LearningTime = FMath::RandRange(TrainingPreset.DurationMin, TrainingPreset.DurationMax);
	if (DebugPanelWidget.IsValid())
		DebugPanelWidget->SetTrainingEpisodeData(TrainingPreset);
	
	if (StartRecordingDelay > 0.f)
	{
		SetState(EMLTrainingSessionState::PreparingToStart);
		TimerManager.SetTimer(StartRecordingDelayTimer, this, &ALearningAgentsImitationCombatRecordingManager::ResumeImitationRecording, StartRecordingDelay);
		if (DebugPanelWidget.IsValid())
		{
			DebugPanelWidget->SetRemainingTime(StartRecordingDelay);
			DebugPanelWidget->SetTimerActive(true);
		}
	}
	else
	{
		ResumeImitationRecording();
	}
}

void ALearningAgentsImitationCombatRecordingManager::StartImitationRecording()
{
	SetState(EMLTrainingSessionState::SettingUpEpisode);
	TrainingEpisodeSetupComponent->SetupEpisode();
}

void ALearningAgentsImitationCombatRecordingManager::StartNextImitationLearning()
{
	RestartImitationRecording(true);
}

void ALearningAgentsImitationCombatRecordingManager::ResumeImitationRecording()
{
	SetState(EMLTrainingSessionState::Running);
	auto& TimerManager = GetWorld()->GetTimerManager();
	ILRecorder->BeginRecording();
	TimerManager.SetTimer(RecordTimer, this, &ALearningAgentsImitationCombatRecordingManager::RecordImitations, RecordInterval, true);
	TimerManager.SetTimer(RestartTimer, this, &ALearningAgentsImitationCombatRecordingManager::StartNextImitationLearning, LearningTime);
	if (DebugPanelWidget.IsValid())
		DebugPanelWidget->SetTimerActive(true);
}

void ALearningAgentsImitationCombatRecordingManager::PauseImitationRecording()
{
	SetState(EMLTrainingSessionState::Paused);
	ILRecorder->EndRecording();
	if (DebugPanelWidget.IsValid())
		DebugPanelWidget->SetTimerActive(false);	
	
	auto& TimerManager = GetWorld()->GetTimerManager();
	LearningTime = TimerManager.GetTimerRemaining(RestartTimer);
	TimerManager.ClearTimer(RecordTimer);
	TimerManager.ClearTimer(RestartTimer);
}

void ALearningAgentsImitationCombatRecordingManager::StopImitationRecording()
{
	PauseImitationRecording();
	SetState(EMLTrainingSessionState::Inactive);
	LearningAgentsManager->ResetAllAgents();
	TrainingEpisodeSetupComponent->Cleanup();
	if (DebugPanelWidget.IsValid())
	{
		DebugPanelWidget->SetRemainingTime(0.f);
		DebugPanelWidget->SetTimerActive(false);
	}
}

void ALearningAgentsImitationCombatRecordingManager::SetState(EMLTrainingSessionState NewState)
{
	TrainingState = NewState;
	TrainingEpisodeStateChangedEvent.Broadcast(NewState);
	if (DebugPanelWidget.IsValid())
		DebugPanelWidget->SetState(NewState);
}

void ALearningAgentsImitationCombatRecordingManager::RecordImitations()
{
	ILController->RunController();
	ILRecorder->AddExperience();
}

void ALearningAgentsImitationCombatRecordingManager::RestartImitationRecording(bool bUseNewSetup)
{
	SetState(EMLTrainingSessionState::Restarting);
	if (DebugPanelWidget.IsValid())
		DebugPanelWidget->SetTimerActive(false);
	
	// TODO (aki) 22.01.2026: consider implementing doing a rollback of recorded data
	ILRecorder->EndRecording();
	
	auto& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(RestartTimer);
	TimerManager.ClearTimer(StartRecordingDelayTimer);
	TimerManager.ClearTimer(RecordTimer);
	
	LearningAgentsManager->ResetAllAgents();
	if (bUseNewSetup)
		TrainingEpisodeSetupComponent->SetupEpisode();
	else 
		TrainingEpisodeSetupComponent->RepeatEpisode();
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

void ALearningAgentsImitationCombatRecordingManager::RegisterMove(AActor* Agent, const FVector& WorldDirection)
{
	FAgentPendingActions& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Locomotion_Move> NewAction = MakeShared<FAction_Locomotion_Move>(GetWorld()->GetTimeSeconds(), WorldDirection);
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

void ALearningAgentsImitationCombatRecordingManager::RegisterAttack(AActor* Agent, uint8 AttackType, UEnum* AttackEnum)
{
	FAgentPendingActions& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Attack> NewAction = MakeShared<FAction_Attack>(GetWorld()->GetTimeSeconds(), AttackType, AttackEnum);
	Queue.AddAction(NewAction);
}

void ALearningAgentsImitationCombatRecordingManager::RegisterParry(AActor* Agent, float ParryAngle)
{
	FAgentPendingActions& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Parry> NewAction = MakeShared<FAction_Parry>(GetWorld()->GetTimeSeconds(), ParryAngle);
	Queue.AddAction(NewAction);
}

void ALearningAgentsImitationCombatRecordingManager::RegisterDodge(AActor* Agent, const FVector& DodgeDirectionWorld)
{
	FAgentPendingActions& Queue = GetAgentActionsQueue(Agent);
	TSharedPtr<FAction_Dodge> NewAction = MakeShared<FAction_Dodge>(GetWorld()->GetTimeSeconds(), DodgeDirectionWorld);
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


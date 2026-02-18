#include "Actors/MLTrainingManager.h"

#include "LearningAgentsInteractor.h"
#include "LearningAgentsManager.h"
#include "Components/TrainingEpisodeManagerComponent.h"
#include "Components/TrainingEpisodeSetupComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MLOverviewPanelWidget.h"

AMLTrainingManager::AMLTrainingManager()
{
	TrainingEpisodeSetupComponent = CreateDefaultSubobject<UTrainingEpisodeSetupComponent>(TEXT("Training Episodes Setup Component"));
}

void AMLTrainingManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	TrainingEpisodeSetupComponent->TrainingEpisodeSetupCompletedEvent.AddUObject(this, &AMLTrainingManager::OnEpisodeSetupCompleted);
	TrainingEpisodeSetupComponent->TrainingEpisodeSetupStepChangedEvent.AddUObject(this, &AMLTrainingManager::OnEpisodeSetupStepChanged);
}

// Called when the game starts or when spawned
void AMLTrainingManager::BeginPlay()
{
	Super::BeginPlay();
	
	auto ManagerPtr = LearningAgentsManager.Get();
	if (IsValid(InteractorClass))
		Interactor = ULearningAgentsInteractor::MakeInteractor(ManagerPtr, InteractorClass, FName("Interactor yobana"));
	
	if (bAddDebugPanelWidget && ensure(!DebugPanelWidgetClass.IsNull()))
	{
		auto WidgetClass = DebugPanelWidgetClass.LoadSynchronous();
		auto PC = UGameplayStatics::GetPlayerController(this, 0);
		DebugPanelWidget = CreateWidget<UMLOverviewPanelWidget>(PC, WidgetClass);
		DebugPanelWidget->AddToViewport();
	}
	
	if (auto GameState = GetWorld()->GetGameState())
		if (auto MLTrainingEpisodeManager = GameState->FindComponentByClass<UTrainingEpisodeManagerComponent>())
			MLTrainingEpisodeManager->RegisterTrainingManager(this);
}

void AMLTrainingManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (auto WorldLocal = GetWorld())
		if (auto GameState = WorldLocal->GetGameState())
			if (auto MLTrainingEpisodeManager = GameState->FindComponentByClass<UTrainingEpisodeManagerComponent>())
				MLTrainingEpisodeManager->UnregisterTrainingManager(this);
	
	Super::EndPlay(EndPlayReason);
}

void AMLTrainingManager::SetState(EMLTrainingSessionState NewState)
{
	TrainingState = NewState;
	TrainingEpisodeStateChangedEvent.Broadcast(NewState);
	if (DebugPanelWidget.IsValid())
		DebugPanelWidget->SetState(NewState);
}

void AMLTrainingManager::StartTraining()
{
	if (TrainingState == EMLTrainingSessionState::Paused)
	{
		ResumeTraining();
	}
	else if (TrainingState == EMLTrainingSessionState::Inactive)
	{
		SetState(EMLTrainingSessionState::SettingUpEpisode);
		TrainingEpisodeSetupComponent->SetupEpisode();
	}
}

void AMLTrainingManager::ResumeTraining()
{
	SetState(EMLTrainingSessionState::Running);
	auto& TimerManager = GetWorld()->GetTimerManager();
	SetActorTickEnabled(true);
	TimerManager.SetTimer(EpisodeTimer, this, &AMLTrainingManager::StartNextEpisode, EpisodeTime);
	if (DebugPanelWidget.IsValid())
	{
		DebugPanelWidget->SetRemainingTime(EpisodeTime);
		DebugPanelWidget->SetTimerActive(true);
	}
}

void AMLTrainingManager::PauseTraining()
{
	SetState(EMLTrainingSessionState::Paused);
	SetActorTickEnabled(false);
	auto& TimerManager = GetWorld()->GetTimerManager();
	EpisodeTime = TimerManager.GetTimerRemaining(EpisodeTimer);
	TimerManager.ClearTimer(EpisodeTimer);
	if (DebugPanelWidget.IsValid())
		DebugPanelWidget->SetTimerActive(false);	
}

void AMLTrainingManager::StopTraining()
{
	PauseTraining();
	SetState(EMLTrainingSessionState::Inactive);
	LearningAgentsManager->ResetAllAgents(); // managers will unregister themselves upon 
	TrainingEpisodeSetupComponent->Stop();
	EpisodeTime = 0.f;
	if (DebugPanelWidget.IsValid())
	{
		DebugPanelWidget->SetRemainingTime(0.f);
		DebugPanelWidget->SetTimerActive(false);
	}
	
	if (auto PlayerController = UGameplayStatics::GetPlayerController(this, 0))
		PlayerController->StartSpectatingOnly();
}

void AMLTrainingManager::RestartTraining(bool bUseNewSetup)
{
	SetState(EMLTrainingSessionState::Restarting);
	if (DebugPanelWidget.IsValid())
		DebugPanelWidget->SetTimerActive(false);
	
	auto& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(EpisodeTimer);
	TimerManager.ClearTimer(StartEpisodeDelayTimer);
	SetActorTickEnabled(false);
	
	LearningAgentsManager->ResetAllAgents();
	if (bUseNewSetup)
		TrainingEpisodeSetupComponent->SetupEpisode();
	else 
		TrainingEpisodeSetupComponent->RepeatEpisode();
}

void AMLTrainingManager::OnEpisodeSetupCompleted(const FMLTrainingPreset& TrainingPreset)
{
	auto& TimerManager = GetWorld()->GetTimerManager();
	EpisodeTime = FMath::RandRange(TrainingPreset.DurationMin, TrainingPreset.DurationMax);
	if (DebugPanelWidget.IsValid())
		DebugPanelWidget->SetTrainingEpisodeData(TrainingPreset);
	
	if (StartRecordingDelay > 0.f)
	{
		SetState(EMLTrainingSessionState::PreparingToStart);
		TimerManager.SetTimer(StartEpisodeDelayTimer, this, &AMLTrainingManager::ResumeTraining, StartRecordingDelay);
		if (DebugPanelWidget.IsValid())
		{
			DebugPanelWidget->SetRemainingTime(StartRecordingDelay);
			DebugPanelWidget->SetTimerActive(true);
		}
	}
	else
	{
		ResumeTraining();
	}
}

void AMLTrainingManager::OnEpisodeSetupStepChanged(ETrainingEpisodeSetupAction TrainingEpisodeSetupAction)
{
	if (DebugPanelWidget.IsValid())
		DebugPanelWidget->OnEpisodeSetupActionChanged(TrainingEpisodeSetupAction);
}

void AMLTrainingManager::StartNextEpisode()
{
	RestartTraining(true);
}

void AMLTrainingManager::SetTrainingEpisodeOrigin_Implementation(const FVector& Origin)
{
	SetActorLocation(Origin);
}

#include "Actors/MLTrainingManager.h"

#include "LearningAgentsInteractor.h"
#include "LearningAgentsManager.h"
#include "PCGComponent.h"
#include "Components/TrainingEpisodeSetupComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MLOverviewPanelWidget.h"


// Sets default values
AMLTrainingManager::AMLTrainingManager()
{
	TrainingEpisodeSetupComponent = CreateDefaultSubobject<UTrainingEpisodeSetupComponent>(TEXT("Training Episodes Setup Component"));
	PCGComponent = CreateDefaultSubobject<UPCGComponent>(TEXT("PCG Component"));
}

void AMLTrainingManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	TrainingEpisodeSetupComponent->SetPCGComponent(PCGComponent);
	TrainingEpisodeSetupComponent->TrainingEpisodeSetupCompletedEvent.AddUObject(this, &AMLTrainingManager::OnEpisodeSetupCompleted);
}

// Called when the game starts or when spawned
void AMLTrainingManager::BeginPlay()
{
	Super::BeginPlay();
	
	auto ManagerPtr = LearningAgentsManager.Get();
	Interactor = ULearningAgentsInteractor::MakeInteractor(ManagerPtr, InteractorClass, FName("Interactor yobana"));
	
	if (bAddDebugPanelWidget && ensure(!DebugPanelWidgetClass.IsNull()))
	{
		auto WidgetClass = DebugPanelWidgetClass.LoadSynchronous();
		auto PC = UGameplayStatics::GetPlayerController(this, 0);
		DebugPanelWidget = CreateWidget<UMLOverviewPanelWidget>(PC, WidgetClass);
		DebugPanelWidget->AddToViewport();
	}
}

// Called every frame
void AMLTrainingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	TimerManager.SetTimer(EpisodeTimer, this, &AMLTrainingManager::StartNextImitationLearning, EpisodeTime);
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
	TrainingEpisodeSetupComponent->Cleanup();
	EpisodeTime = 0.f;
	if (DebugPanelWidget.IsValid())
	{
		DebugPanelWidget->SetRemainingTime(0.f);
		DebugPanelWidget->SetTimerActive(false);
	}
}

void AMLTrainingManager::RestartTraining(bool bUseNewSetup)
{
	SetState(EMLTrainingSessionState::Restarting);
	if (DebugPanelWidget.IsValid())
		DebugPanelWidget->SetTimerActive(false);
	
	auto& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(EpisodeTimer);
	TimerManager.ClearTimer(StartRecordingDelayTimer);
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
		TimerManager.SetTimer(StartRecordingDelayTimer, this, &AMLTrainingManager::ResumeTraining, StartRecordingDelay);
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

void AMLTrainingManager::StartNextImitationLearning()
{
	RestartTraining(true);
}

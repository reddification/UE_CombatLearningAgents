// 


#include "Actors/IL/LearningAgentsImitationCombatRecordingManager.h"

#include "LearningAgentsController.h"
#include "LearningAgentsInteractor.h"
#include "LearningAgentsManager.h"
#include "LearningAgentsRecorder.h"
#include "Subsystems/LearningAgentSubsystem.h"

// Sets default values
ALearningAgentsImitationCombatRecordingManager::ALearningAgentsImitationCombatRecordingManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
	LearningAgentsManager = CreateDefaultSubobject<ULearningAgentsManager>(TEXT("LearningAgentsManager"));
}

// Called when the game starts or when spawned
void ALearningAgentsImitationCombatRecordingManager::BeginPlay()
{
	Super::BeginPlay();
	auto LAS = GetWorld()->GetSubsystem<ULearningAgentSubsystem>();
	LAS->RegisterLearningAgentsManager(LearningAgentsManager);

	Interactor = ULearningAgentsInteractor::MakeInteractor(LearningAgentsManager, InteractorClass, FName("RecorderCombatInteractor"));
	auto InteractorPtr = Interactor.Get();

	ILController = ULearningAgentsController::MakeController(LearningAgentsManager, InteractorPtr, ILControllerClass);
	bool bReinitializeRecording = true;
	ILRecorder = ULearningAgentsRecorder::MakeRecorder(LearningAgentsManager, InteractorPtr, ILRecorderClass, FName("ILCombatRecorder"),
	                                                   RecorderPathSettings, RecordingAsset, bReinitializeRecording);

	ILRecorder->BeginRecording();
}

void ALearningAgentsImitationCombatRecordingManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (auto World = GetWorld())
		if (auto LAS = World->GetSubsystem<ULearningAgentSubsystem>())
			LAS->UnregisterLearningAgentsManager(LearningAgentsManager);

	if (ILRecorder != nullptr)
		ILRecorder->EndRecording();
	
	Super::EndPlay(EndPlayReason);
}

void ALearningAgentsImitationCombatRecordingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ILController->RunController();
	ILRecorder->AddExperience();

	AccumulatedTime += DeltaTime;
	if (AccumulatedTime > LearningTime)
	{
		ILRecorder->EndRecording();
		LearningAgentsManager->ResetAllAgents();
		AccumulatedTime = 0.f;
		ResetLearningAgents();
		ILRecorder->BeginRecording();
	}
}

void ALearningAgentsImitationCombatRecordingManager::ResetLearningAgents()
{
	ensure(false); // TODO
}


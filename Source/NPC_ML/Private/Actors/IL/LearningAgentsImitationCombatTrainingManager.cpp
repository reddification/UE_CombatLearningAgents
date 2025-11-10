// 


#include "Actors/IL/LearningAgentsImitationCombatTrainingManager.h"

#include "LearningAgentsCommunicator.h"
#include "LearningAgentsImitationTrainer.h"
#include "LearningAgentsInteractor.h"
#include "LearningAgentsManager.h"
#include "LearningAgentsPolicy.h"
#include "Subsystems/LearningAgentSubsystem.h"


// Sets default values
ALearningAgentsImitationCombatTrainingManager::ALearningAgentsImitationCombatTrainingManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
	LearningAgentsManager = CreateDefaultSubobject<ULearningAgentsManager>(TEXT("LearningAgentsManager"));
}

// Called when the game starts or when spawned
void ALearningAgentsImitationCombatTrainingManager::BeginPlay()
{
	Super::BeginPlay();
	auto LAS = GetWorld()->GetSubsystem<ULearningAgentSubsystem>();
	LAS->RegisterLearningAgentsManager(LearningAgentsManager);

	Interactor = ULearningAgentsInteractor::MakeInteractor(LearningAgentsManager, InteractorClass, FName("RecorderCombatInteractor"));
	auto InteractorPtr = Interactor.Get();

	Policy = ULearningAgentsPolicy::MakePolicy(
		LearningAgentsManager, InteractorPtr, PolicyClass, FName("CombatPolicy"),
		EncoderNN, PolicyNN, DecoderNN,
		bReinitializeWeights, bReinitializeWeights, bReinitializeWeights,
		PolicySettings, Seed
		);

	auto PolicyPtr = Policy.Get();
	SharedMemory = ULearningAgentsCommunicatorLibrary::SpawnSharedMemoryTrainingProcess(TrainerProcessSettings, SharedMemorySettings);
	Communicator = ULearningAgentsCommunicatorLibrary::MakeSharedMemoryCommunicator(SharedMemory);

	ImitationTrainer = ULearningAgentsImitationTrainer::MakeImitationTrainer(LearningAgentsManager, InteractorPtr, PolicyPtr, Communicator,
		ImitationTrainerClass, FName("CombatImitationTrainer"));
}

void ALearningAgentsImitationCombatTrainingManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (auto World = GetWorld())
		if (auto LAS = World->GetSubsystem<ULearningAgentSubsystem>())
			LAS->UnregisterLearningAgentsManager(LearningAgentsManager);
	
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ALearningAgentsImitationCombatTrainingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ImitationTrainer->RunTraining(RecordingAsset, ImitationTrainerSettings, ImitationTrainerTrainingSettings, ImitationTrainerPathSettings);
	Policy->RunInference();
	AccumulatedTime += DeltaTime;
	if (AccumulatedTime >= AccumulationTimeInterval)
	{
		AccumulatedTime = 0.f;
		LearningAgentsManager->ResetAllAgents();
		ResetTestingGameEnvironment();
	}
}

void ALearningAgentsImitationCombatTrainingManager::ResetTestingGameEnvironment()
{
	// TODO implement
	ensure(false);
}

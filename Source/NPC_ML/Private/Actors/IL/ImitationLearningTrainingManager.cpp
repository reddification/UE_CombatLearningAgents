// 


#include "Actors/IL/ImitationLearningTrainingManager.h"

#include "LearningAgentsCommunicator.h"
#include "LearningAgentsImitationTrainer.h"
#include "LearningAgentsInteractor.h"
#include "LearningAgentsManager.h"
#include "LearningAgentsPolicy.h"
#include "Data/LearningAgentsTags_Combat.h"
#include "Subsystems/MLSubsystem.h"


// Sets default values
AImitationLearningTrainingManager::AImitationLearningTrainingManager()
{
	BehaviorTag = LearningAgentsTags_Combat::Behavior_Combat_IL_Training;
}

// Called when the game starts or when spawned
void AImitationLearningTrainingManager::BeginPlay()
{
	Super::BeginPlay();

	auto ManagerPtr = LearningAgentsManager.Get();
	Interactor = ULearningAgentsInteractor::MakeInteractor(ManagerPtr, InteractorClass, FName("RecorderCombatInteractor"));
	auto InteractorPtr = Interactor.Get();

	Policy = ULearningAgentsPolicy::MakePolicy(
		ManagerPtr, InteractorPtr, PolicyClass, FName("CombatPolicy"),
		EncoderNN, PolicyNN, DecoderNN,
		bReinitializeWeights, bReinitializeWeights, bReinitializeWeights,
		PolicySettings, Seed
		);

	auto PolicyPtr = Policy.Get();
	SharedMemory = ULearningAgentsCommunicatorLibrary::SpawnSharedMemoryTrainingProcess(TrainerProcessSettings, SharedMemorySettings);
	Communicator = ULearningAgentsCommunicatorLibrary::MakeSharedMemoryCommunicator(SharedMemory);

	ImitationTrainer = ULearningAgentsImitationTrainer::MakeImitationTrainer(ManagerPtr, InteractorPtr, PolicyPtr, Communicator,
		ImitationTrainerClass, FName("CombatImitationTrainer"));
}

// Called every frame
void AImitationLearningTrainingManager::Tick(float DeltaTime)
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

void AImitationLearningTrainingManager::ResetTestingGameEnvironment()
{
	// TODO implement
	ensure(false);
}

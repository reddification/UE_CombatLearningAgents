#include "Actors/RL/LAReinforcementLearningManager.h"

#include "LearningAgentsInteractor.h"
#include "LearningAgentsTrainingEnvironment.h"
#include "Data/LearningAgentsTags_Combat.h"
#include "Subsystems/MLSubsystem.h"

ALAReinforcementLearningManager::ALAReinforcementLearningManager()
{
	BehaviorTag = LearningAgentsTags_Combat::Behavior_Combat_RL;
}

void ALAReinforcementLearningManager::BeginPlay()
{
	Super::BeginPlay();
	bool bAllSet = IsValid(InteractorClass) && IsValid(TrainingEnvironmentClass)
		&& IsValid(PolicyClass) && IsValid(EncoderNN) && IsValid(DecoderNN) && IsValid(PolicyNN)
		&& IsValid(CriticClass) && IsValid(CriticNN);
	
	if (!ensure(bAllSet))
		return;
	
	ULearningAgentsManager* LAM = LearningAgentsManager.Get();
	auto InteractorPtr = Interactor.Get();
	Policy = ULearningAgentsPolicy::MakePolicy(
		LAM, InteractorPtr, PolicyClass, FName("CombatPolicy"),
		EncoderNN, PolicyNN, DecoderNN,
		bReinitializeWeights,  bReinitializeWeights,  bReinitializeWeights,
		PolicySettings, Seed
		);

	auto PolicyPtr = Policy.Get();
	Critic = ULearningAgentsCritic::MakeCritic(LAM, InteractorPtr, PolicyPtr, CriticClass, FName("CombatCritic"),
		CriticNN, bReinitializeWeights, CriticSettings, Seed);
	auto CriticPtr = Critic.Get();

	TrainingEnvironment = ULearningAgentsTrainingEnvironment::MakeTrainingEnvironment(LAM,
		TrainingEnvironmentClass, FName("CombatTrainingEnvironment"));
	auto TrainingEnvironmentPtr = TrainingEnvironment.Get();

	SharedMemory = ULearningAgentsCommunicatorLibrary::SpawnSharedMemoryTrainingProcess(TrainerProcessSettings, SharedMemorySettings);
	Communicator = ULearningAgentsCommunicatorLibrary::MakeSharedMemoryCommunicator(SharedMemory);

	PPOTrainer = ULearningAgentsPPOTrainer::MakePPOTrainer(LAM, InteractorPtr, TrainingEnvironmentPtr, PolicyPtr,
		CriticPtr, Communicator, PPOTrainerClass, FName("Combat PPO Trainer"), PPOTrainerSettings);
}

void ALAReinforcementLearningManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PPOTrainer->RunTraining(TrainerTrainingSettings, TrainingGameSettings);
}


#include "Actors/RL/LAReinforcementLearningManager.h"

#include "LearningAgentsInteractor.h"
#include "LearningAgentsTrainingEnvironment.h"
#include "Data/LearningAgentsTags_Combat.h"
#include "Data/MLModelVersion.h"
#include "Data/MLTrainingConfigurationBase.h"
#include "Subsystems/MLSubsystem.h"

ALAReinforcementLearningManager::ALAReinforcementLearningManager()
{
	BehaviorTag = LearningAgentsTags_Combat::Behavior_Combat_RL;
}

void ALAReinforcementLearningManager::BeginPlay()
{
	Super::BeginPlay();
	
	RLConfiguration = Cast<UMLTrainingConfiguration_RL>(TrainingConfiguration);
	if (!RLConfiguration || RLConfiguration->ModelVersion.IsNull())
		{ ensure(false); return; }
	
	UMLModelVersion* MLModelVersion = RLConfiguration->ModelVersion.LoadSynchronous(); 
	bool bAllSet = MLModelVersion->IsSet() && !MLModelVersion->CriticNN.IsNull();
	if (!ensure(bAllSet))
		return;
	
	ULearningAgentsManager* LAM = LearningAgentsManager.Get();
	auto InteractorPtr = Interactor.Get();
	Policy = ULearningAgentsPolicy::MakePolicy(
		LAM, InteractorPtr, MLModelVersion->PolicyClass, FName("CombatPolicy"),
		MLModelVersion->EncoderNN, MLModelVersion->PolicyNN, MLModelVersion->DecoderNN,
		false,  false,  false,
		MLModelVersion->PolicySettings, RLConfiguration->Seed);

	auto PolicyPtr = Policy.Get();
	auto CriticNN = MLModelVersion->CriticNN.LoadSynchronous();
	Critic = ULearningAgentsCritic::MakeCritic(LAM, InteractorPtr, PolicyPtr, MLModelVersion->CriticClass, FName("CombatCritic"),
		CriticNN, false, MLModelVersion->CriticSettings, RLConfiguration->Seed);
	auto CriticPtr = Critic.Get();

	TrainingEnvironment = ULearningAgentsTrainingEnvironment::MakeTrainingEnvironment(LAM,
		RLConfiguration->TrainingEnvironmentClass, FName("CombatTrainingEnvironment"));
	auto TrainingEnvironmentPtr = TrainingEnvironment.Get();

	SharedMemory = ULearningAgentsCommunicatorLibrary::SpawnSharedMemoryTrainingProcess(RLConfiguration->TrainerProcessSettings, RLConfiguration->SharedMemorySettings);
	Communicator = ULearningAgentsCommunicatorLibrary::MakeSharedMemoryCommunicator(SharedMemory);

	PPOTrainer = ULearningAgentsPPOTrainer::MakePPOTrainer(LAM, InteractorPtr, TrainingEnvironmentPtr, PolicyPtr,
		CriticPtr, Communicator, RLConfiguration->PPOTrainerClass, FName("Combat PPO Trainer"), RLConfiguration->PPOTrainerSettings);
}

void ALAReinforcementLearningManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PPOTrainer->RunTraining(RLConfiguration->TrainerTrainingSettings, RLConfiguration->TrainingGameSettings);
}


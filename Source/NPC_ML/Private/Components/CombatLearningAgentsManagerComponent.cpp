#include "Components/CombatLearningAgentsManagerComponent.h"

#include "LearningAgentsCommunicator.h"
#include "LearningAgentsCritic.h"
#include "LearningAgentsInteractor.h"
#include "LearningAgentsPolicy.h"
#include "LearningAgentsPPOTrainer.h"
#include "LearningAgentsTrainingEnvironment.h"
#include "Subsystems/LearningAgentSubsystem.h"


UCombatLearningAgentsManagerComponent::UCombatLearningAgentsManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UCombatLearningAgentsManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	bool bAllSet = IsValid(InteractorClass) && IsValid(TrainingEnvironmentClass)
		&& IsValid(PolicyClass) && IsValid(EncoderNN) && IsValid(DecoderNN) && IsValid(PolicyNN)
		&& IsValid(CriticClass) && IsValid(CriticNN);
	
	if (!ensure(bAllSet))
		return;
	
	auto LearningAgentSubsystem = GetWorld()->GetSubsystem<ULearningAgentSubsystem>();
	LearningAgentSubsystem->RegisterLearningAgentsManager(this);

	// дебилы, блять. нахуя они просят рефом, притом не константным, поинтер кидать внутрь фреймворка?
	// я смотрел код у них там внутри. они нигде этим рефам ничего не присваивают, просто сохраняют в свои переменные эти поинтеры. ебанутые, блять.
	// как будто питоно-обезьян, умеющих только импортить МЛ-либы в питоне, посадили писать плагин для анриала на плюсах,
	// и вот они на нормальном ЯП в первые раз жизни писали и нахуярили как получилось
	ULearningAgentsManager* ManagerPtr = this;
	
	Interactor = ULearningAgentsInteractor::MakeInteractor(ManagerPtr, InteractorClass, FName("CombatInteractor"));
	auto InteractorPtr = Interactor.Get();
	Policy = ULearningAgentsPolicy::MakePolicy(
		ManagerPtr, InteractorPtr, PolicyClass, FName("CombatPolicy"),
		EncoderNN, PolicyNN, DecoderNN,
		!bRunInference && bReinitializeWeights, !bRunInference && bReinitializeWeights, !bRunInference && bReinitializeWeights,
		PolicySettings, Seed
		);

	auto PolicyPtr = Policy.Get();
	if (!bRunInference)
	{
		Critic = ULearningAgentsCritic::MakeCritic(ManagerPtr, InteractorPtr, PolicyPtr, CriticClass, FName("CombatCritic"),
		CriticNN, !bRunInference && bReinitializeWeights, CriticSettings, Seed);
		auto CriticPtr = Critic.Get();
	
		TrainingEnvironment = ULearningAgentsTrainingEnvironment::MakeTrainingEnvironment(ManagerPtr, TrainingEnvironmentClass, FName("CombatTrainingEnvironment"));
		auto TrainingEnvironmentPtr = TrainingEnvironment.Get();
	
		SharedMemory = ULearningAgentsCommunicatorLibrary::SpawnSharedMemoryTrainingProcess(TrainerProcessSettings, SharedMemorySettings);
		Communicator = ULearningAgentsCommunicatorLibrary::MakeSharedMemoryCommunicator(SharedMemory);

		PPOTrainer = ULearningAgentsPPOTrainer::MakePPOTrainer(ManagerPtr, InteractorPtr, TrainingEnvironmentPtr, PolicyPtr,
			CriticPtr, Communicator, PPOTrainerClass, FName("Combat PPO Trainer"), PPOTrainerSettings);

		
	}
}

void UCombatLearningAgentsManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (auto World = GetWorld())
		if (auto LearningAgentSubsystem = World->GetSubsystem<ULearningAgentSubsystem>())
			LearningAgentSubsystem->UnregisterLearningAgentsManager(this);
	
	Super::EndPlay(EndPlayReason);
}


// Called every frame
void UCombatLearningAgentsManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bRunInference)
		Policy->RunInference();
	else
		PPOTrainer->RunTraining(TrainerTrainingSettings, TrainingGameSettings);
}


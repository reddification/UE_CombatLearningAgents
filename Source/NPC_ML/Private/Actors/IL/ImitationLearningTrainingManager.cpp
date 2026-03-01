#include "Actors/IL/ImitationLearningTrainingManager.h"

#include "LearningAgentsCommunicator.h"
#include "LearningAgentsImitationTrainer.h"
#include "LearningAgentsInteractor.h"
#include "LearningAgentsManager.h"
#include "LearningAgentsPolicy.h"
#include "LearningAgentsRecording.h"
#include "Data/LearningAgentsTags_Combat.h"
#include "Data/MLModelVersion.h"
#include "Data/MLTrainingConfigurationBase.h"
#include "Subsystems/MLSubsystem.h"

AImitationLearningTrainingManager::AImitationLearningTrainingManager()
{
	BehaviorTag = LearningAgentsTags_Combat::Behavior_Combat_IL_Training;
	PrimaryActorTick.TickInterval = 0.f;
}

void AImitationLearningTrainingManager::BeginPlay()
{
	Super::BeginPlay();

	ILConfiguration = Cast<UMLTrainingConfiguration_IL>(TrainingConfiguration);
	if (!ILConfiguration || ILConfiguration->ModelVersion.IsNull())
		{ ensure(false); return; }
	
	auto ModelVersion = ILConfiguration->ModelVersion.LoadSynchronous();
	if (!ModelVersion->IsSet() || ModelVersion->RecordingAsset.IsNull())
		{ ensure(false); return; }
		
	RecordingAsset = ModelVersion->RecordingAsset.LoadSynchronous();
	auto ManagerPtr = LearningAgentsManager.Get();
	auto InteractorPtr = Interactor.Get();

	Policy = ULearningAgentsPolicy::MakePolicy(
		ManagerPtr, InteractorPtr, ModelVersion->PolicyClass, FName("CombatPolicy"),
		ModelVersion->EncoderNN, ModelVersion->PolicyNN, ModelVersion->DecoderNN,
		false, false, false,
		ModelVersion->PolicySettings, ILConfiguration->Seed);

	auto PolicyPtr = Policy.Get();
	SharedMemory = ULearningAgentsCommunicatorLibrary::SpawnSharedMemoryTrainingProcess(ILConfiguration->TrainerProcessSettings, ILConfiguration->SharedMemorySettings);
	Communicator = ULearningAgentsCommunicatorLibrary::MakeSharedMemoryCommunicator(SharedMemory);

	ImitationTrainer = ULearningAgentsImitationTrainer::MakeImitationTrainer(ManagerPtr, InteractorPtr, PolicyPtr, Communicator,
		ILConfiguration->ImitationTrainerClass, FName("CombatImitationTrainer"));
}

void AImitationLearningTrainingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsTrainingActive())
		return;
	
	const bool bWasTraining = (ImitationTrainer != nullptr) && ImitationTrainer->IsTraining();
	
	ImitationTrainer->RunTraining(RecordingAsset, ILConfiguration->TrainerSettings, ILConfiguration->TrainerTrainingSettings, ILConfiguration->TrainerProcessSettings);
	
	if (ILConfiguration->bDoLivePreview)
	{
		DelayTillNextInference -= DeltaTime;
		if (DelayTillNextInference <= 0.f)
		{
			Policy->RunInference();
			DelayTillNextInference = ILConfiguration->InferenceTickRate;
		}
	}
	
	const bool bIsTrainingAfter = (ImitationTrainer != nullptr) && ImitationTrainer->IsTraining();
	if (bWasTraining && !bIsTrainingAfter)
		OnTrainingFinished();
}

void AImitationLearningTrainingManager::OnTrainingFinished()
{
	int wtf_do_i_do_now = 1;
}

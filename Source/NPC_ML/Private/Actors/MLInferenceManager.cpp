// 


#include "Actors/MLInferenceManager.h"

#include "LearningAgentsInteractor.h"
#include "Data/LearningAgentsTags_Combat.h"
#include "Data/MLModelVersion.h"


// Sets default values
AMLInferenceManager::AMLInferenceManager()
{
	BehaviorTag = LearningAgentsTags_Combat::Behavior_Combat_Inference;
}

// Called when the game starts or when spawned
void AMLInferenceManager::BeginPlay()
{
	Super::BeginPlay();
	if (Model.IsNull())
		return;
	
	auto ModelVersion = Model.LoadSynchronous();
	if (!ensure(ModelVersion->IsSet()))
		return;
	
	// дебилы, блять. нахуя они просят рефом, притом не константным, поинтер кидать внутрь фреймворка?
	// я смотрел код у них там внутри. они нигде этим рефам ничего не присваивают, просто сохраняют в свои переменные эти поинтеры. ебанутые, блять.
	// как будто питоно-обезьян, умеющих только импортить МЛ-либы в питоне, посадили писать плагин для анриала на плюсах,
	// и вот они на нормальном ЯП в первые раз жизни писали и нахуярили как получилось
	ULearningAgentsManager* ManagerPtr = LearningAgentsManager;
	Interactor = ModelVersion->MakeInteractor(LearningAgentsManager.Get());
	auto InteractorPtr = Interactor.Get();
	Policy = ULearningAgentsPolicy::MakePolicy(
		ManagerPtr, InteractorPtr, ModelVersion->PolicyClass, FName("CombatPolicy"),
		ModelVersion->EncoderNN, ModelVersion->PolicyNN, ModelVersion->DecoderNN,
		false,  false,  false,
		ModelVersion->PolicySettings, Seed);
}

void AMLInferenceManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Policy->RunInference();
}


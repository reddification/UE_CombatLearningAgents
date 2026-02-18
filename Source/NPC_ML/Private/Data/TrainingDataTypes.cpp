#include "Data/TrainingDataTypes.h"
#include "EnvironmentQuery/EnvQuery.h"

#if WITH_EDITOR

void UMLTrainingEpisodeActorTemplate::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (IsValid(SpawnActorLocationEQS.QueryTemplate))
		SpawnActorLocationEQS.QueryTemplate->CollectQueryParams(*this, SpawnActorLocationEQS.QueryConfig);

	if (IsValid(InitialActorLookAtEQS.QueryTemplate))
		InitialActorLookAtEQS.QueryTemplate->CollectQueryParams(*this, InitialActorLookAtEQS.QueryConfig);

	// TODO 27.01.2026 (aki): interface or base virtual function like UpdateEditorConfig(*this) for whatever setup actions using EQS or whatever
	// for (auto& SpawnActionInstancedStruct : SpawnDescriptor.SetupPipeline)
	// {
	// }
}

#endif
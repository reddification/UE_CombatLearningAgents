#include "Data/MLTrainingPresetsDataAsset.h"

#include "Data/TrainingDataTypes.h"
#include "EnvironmentQuery/EnvQuery.h"

#if WITH_EDITOR

void UMLTrainingPresetsDataAsset::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	for (auto& TrainingPreset : TrainingPresets)
	{
		if (IsValid(TrainingPreset.EpisodeOriginLocationEQS.QueryTemplate))
			TrainingPreset.EpisodeOriginLocationEQS.QueryTemplate->CollectQueryParams(*this, TrainingPreset.EpisodeOriginLocationEQS.QueryConfig);
		
		for (auto& SpawnDescriptor : TrainingPreset.ActorsSpawnDescriptors)
		{
			if (IsValid(SpawnDescriptor.SpawnActorLocationEQS.QueryTemplate))
				SpawnDescriptor.SpawnActorLocationEQS.QueryTemplate->CollectQueryParams(*this, SpawnDescriptor.SpawnActorLocationEQS.QueryConfig);
			
			if (IsValid(SpawnDescriptor.InitialActorLookAtEQS.QueryTemplate))
				SpawnDescriptor.InitialActorLookAtEQS.QueryTemplate->CollectQueryParams(*this, SpawnDescriptor.InitialActorLookAtEQS.QueryConfig);
			
			// TODO 27.01.2026 (aki): interface or base virtual function like UpdateEditorConfig(*this) for whatever setup actions using EQS or whatever
			// for (auto& SpawnActionInstancedStruct : SpawnDescriptor.SetupPipeline)
			// {
			// }
		}
	}
}

#endif
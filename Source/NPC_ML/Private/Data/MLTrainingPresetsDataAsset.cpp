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
	}
}

#endif
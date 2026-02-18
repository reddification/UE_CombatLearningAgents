#include "Components/TrainingEpisodeManagerComponent.h"

#include "Actors/MLTrainingManager.h"
#include "Data/LogChannels.h"

void UTrainingEpisodeManagerComponent::RegisterTrainingManager(const AMLTrainingManager* NewTrainingManager)
{
	if (NewTrainingManager == nullptr) return;
	if (TrainingManager.IsValid())
	{
		UE_VLOG_UELOG(this, LogNpcMl, Warning, 
			TEXT("UTrainingEpisodeManagerComponent::RegisterTrainingManager: attempt to register a new training manager when one already exists"))
		TrainingManager->TrainingEpisodeStateChangedEvent.RemoveAll(this);
	}
	
	TrainingManager = NewTrainingManager;
	TrainingManager->TrainingEpisodeStateChangedEvent.AddUObject(this, &UTrainingEpisodeManagerComponent::OnTrainingEpisodeStateChanged);
}

void UTrainingEpisodeManagerComponent::UnregisterTrainingManager(const AMLTrainingManager* OldTrainingManager)
{
	if (OldTrainingManager == nullptr || OldTrainingManager != TrainingManager) return;
	TrainingManager->TrainingEpisodeStateChangedEvent.RemoveAll(this);
	TrainingManager.Reset();
}
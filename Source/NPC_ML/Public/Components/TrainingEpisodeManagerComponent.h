// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/TrainingDataTypes.h"
#include "TrainingEpisodeManagerComponent.generated.h"


class AMLTrainingManager;

/*
 * Put a child of this component on your training map game state if you want to handle training episode state changes
 * For example, you can disable non-ML NPCs brains when episode is paused or is preparing to start 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NPC_ML_API UTrainingEpisodeManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void RegisterTrainingManager(const AMLTrainingManager* NewTrainingManager);
	void UnregisterTrainingManager(const AMLTrainingManager* OldTrainingManager);

protected:
	virtual void OnTrainingEpisodeStateChanged(EMLTrainingSessionState NewTrainingSessionState) {};
	
private:
	TWeakObjectPtr<const AMLTrainingManager> TrainingManager;
};

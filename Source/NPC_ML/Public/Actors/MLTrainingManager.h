#pragma once

#include "CoreMinimal.h"
#include "Actors/MLManagerBase.h"
#include "Components/TrainingEpisodeSetupComponent.h"
#include "Data/TrainingDataTypes.h"
#include "Interfaces/MLTrainingManagerPcgOwnerInterface.h"
#include "MLTrainingManager.generated.h"

class UTrainingEpisodeSetupComponent;
class UPCGComponent;
class ULearningAgentsInteractor;
class UMLOverviewPanelWidget;

UCLASS()
class NPC_ML_API AMLTrainingManager : public AMLManagerBase, public IMLTrainingManagerPcgOwnerInterface
{
	GENERATED_BODY()

private:
	DECLARE_MULTICAST_DELEGATE_OneParam(FTrainingEpisodeStateChangedEvent, EMLTrainingSessionState);

public:
	// Sets default values for this actor's properties
	AMLTrainingManager();
	virtual void PostInitializeComponents() override;
	
	FTrainingEpisodeStateChangedEvent TrainingEpisodeStateChangedEvent;
	
	FORCEINLINE bool IsTrainingActive() const { return TrainingState == EMLTrainingSessionState::Running; };
	
	virtual void StartTraining();
	virtual void ResumeTraining();
	virtual void PauseTraining();
	virtual void StopTraining();
	virtual void RestartTraining(bool bUseNewSetup);
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UTrainingEpisodeSetupComponent> TrainingEpisodeSetupComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ULearningAgentsInteractor> InteractorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<UMLOverviewPanelWidget> DebugPanelWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = 0.001f, ClampMin = 0.001f))
	float StartRecordingDelay = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAddDebugPanelWidget = true;

	virtual void SetState(EMLTrainingSessionState NewState);
	
	EMLTrainingSessionState TrainingState = EMLTrainingSessionState::Inactive;
	TWeakObjectPtr<UMLOverviewPanelWidget> DebugPanelWidget;
	
	UPROPERTY()
	TObjectPtr<ULearningAgentsInteractor> Interactor;
	
	float EpisodeTime = 0.f;
	virtual void OnEpisodeSetupCompleted(const FMLTrainingPreset& TrainingPreset);
	virtual void OnEpisodeSetupStepChanged(ETrainingEpisodeSetupAction TrainingEpisodeSetupAction);
	virtual void StartNextEpisode();

private:
	FTimerHandle EpisodeTimer;
	FTimerHandle StartEpisodeDelayTimer;
	
public: // IMLTrainingManagerPcgOwnerInterface
	virtual void SetTrainingEpisodeOrigin_Implementation(const FVector& Origin) override;
};

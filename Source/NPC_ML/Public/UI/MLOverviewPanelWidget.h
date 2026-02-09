// 

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/TrainingDataTypes.h"
#include "MLOverviewPanelWidget.generated.h"

class UHorizontalBox;
class UTextBlock;
enum class EMLTrainingSessionState;
/**
 * 
 */
UCLASS()
class NPC_ML_API UMLOverviewPanelWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetState(EMLTrainingSessionState NewState);
	void SetRemainingTime(float RemainingTime);
	void SetTimerActive(bool bActive);
	void SetTrainingEpisodeData(const FMLTrainingPreset& TrainingPreset);
	void OnEpisodeSetupActionChanged(ETrainingEpisodeSetupAction TrainingEpisodeSetupAction);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EMLTrainingSessionState, FLinearColor> SessionStateColors;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EMLTrainingSessionState, FText> StatesTitles;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ETrainingEpisodeSetupAction, FText> TrainingEpisodeSetupActionTitles;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* RemainingTimeTextblock;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* StatusTextblock;

	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* EpisodeStepContainer;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TrainingEpisodeSetupActionTextblock;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TrainingEpisodeTitleTextblock;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TrainingEpisodeDescriptionTextblock;
	
private:
	FTimerHandle StateTimer;
	float RemainingTime = 0.0f;
	
	void UpdateStateTimer();
};

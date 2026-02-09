#include "UI/MLOverviewPanelWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

void UMLOverviewPanelWidget::SetState(EMLTrainingSessionState NewState)
{
	const FLinearColor* StateColor = SessionStateColors.Find(NewState);
	if (ensure(StateColor))
		StatusTextblock->SetColorAndOpacity(*StateColor);
	
	const FText* StateTitle = StatesTitles.Find(NewState);
	if (ensure(StateTitle))
		StatusTextblock->SetText(*StateTitle);	
}

void UMLOverviewPanelWidget::SetRemainingTime(float InRemainingTime)
{
	RemainingTime = InRemainingTime;
	RemainingTimeTextblock->SetText(FText::AsNumber(FMath::FloorToInt32(RemainingTime)));
}

void UMLOverviewPanelWidget::SetTimerActive(bool bActive)
{
	auto& TimerManager = GetWorld()->GetTimerManager();
	if (bActive)
		TimerManager.SetTimer(StateTimer, this, &UMLOverviewPanelWidget::UpdateStateTimer, 1.f, true);
	else 
		TimerManager.ClearTimer(StateTimer);
}

void UMLOverviewPanelWidget::SetTrainingEpisodeData(const FMLTrainingPreset& TrainingPreset)
{
	TrainingEpisodeTitleTextblock->SetText(FText::FromString(TrainingPreset.ConfigTitle));
	TrainingEpisodeDescriptionTextblock->SetText(FText::FromString(TrainingPreset.ConfigDescription));
}

void UMLOverviewPanelWidget::UpdateStateTimer()
{
	RemainingTime -= 1.f;
	if (RemainingTime < 0.f)
		GetWorld()->GetTimerManager().ClearTimer(StateTimer);
	
	RemainingTimeTextblock->SetText(FText::AsNumber(FMath::FloorToInt32(RemainingTime)));
}

void UMLOverviewPanelWidget::OnEpisodeSetupActionChanged(ETrainingEpisodeSetupAction TrainingEpisodeSetupAction)
{
	auto NewVisibility = TrainingEpisodeSetupAction == ETrainingEpisodeSetupAction::None ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible;
	if (GetVisibility() != NewVisibility)
		EpisodeStepContainer->SetVisibility(NewVisibility);

	if (auto DescriptionTextPtr = TrainingEpisodeSetupActionTitles.Find(TrainingEpisodeSetupAction))
		TrainingEpisodeSetupActionTextblock->SetText(*DescriptionTextPtr);
}
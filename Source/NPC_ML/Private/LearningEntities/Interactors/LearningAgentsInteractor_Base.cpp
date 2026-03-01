#include "LearningEntities/Interactors/LearningAgentsInteractor_Base.h"

void ULearningAgentsInteractor_Base::SetRelevantObservations(const FGameplayTagContainer& InObservations)
{
	RelevantObservations = InObservations;
}

void ULearningAgentsInteractor_Base::SetRelevantActions(const FGameplayTagContainer& InActions)
{
	RelevantActions = InActions;
}

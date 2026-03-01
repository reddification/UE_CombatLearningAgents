// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LearningAgentsInteractor.h"
#include "LearningAgentsInteractor_Base.generated.h"

/**
 * 
 */
UCLASS()
class NPC_ML_API ULearningAgentsInteractor_Base : public ULearningAgentsInteractor
{
	GENERATED_BODY()
	
public:
	void SetRelevantObservations(const FGameplayTagContainer& InObservations);
	void SetRelevantActions(const FGameplayTagContainer& InActions);
	
protected:
	// Use these to control what observations are made by this interactor  
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer RelevantObservations;
	
	// Use these to control what actions are made by this interactor  
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer RelevantActions;
};

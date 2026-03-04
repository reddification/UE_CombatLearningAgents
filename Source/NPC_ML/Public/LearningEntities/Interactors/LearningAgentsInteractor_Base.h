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
	FORCEINLINE void SetRelevantObservations(const FGameplayTagContainer& InObservations) { RelevantObservations = InObservations; }
	FORCEINLINE void SetRelevantActions(const FGameplayTagContainer& InActions) { RelevantActions = InActions; }
	FORCEINLINE void SetImitationRecordingModeActive(bool bActive) { bImitationRecordingMode = bActive; }
	
	static ULearningAgentsInteractor_Base* MakeUninitializedInteractor(ULearningAgentsManager* LAM, const TSubclassOf<ULearningAgentsInteractor_Base>& InteractorClass,
		const FName& InteractorName);
	
	virtual void SpecifyAgentObservation_Implementation(FLearningAgentsObservationSchemaElement& OutObservationSchemaElement, ULearningAgentsObservationSchema* InObservationSchema) override;
	virtual void SpecifyAgentAction_Implementation(FLearningAgentsActionSchemaElement& OutActionSchemaElement, ULearningAgentsActionSchema* InActionSchema) override;
	virtual void GatherAgentObservation_Implementation(FLearningAgentsObservationObjectElement& OutObservationObjectElement, ULearningAgentsObservationObject* InObservationObject, const int32 AgentId) override;
	virtual void PerformAgentAction_Implementation(const ULearningAgentsActionObject* InActionObject, const FLearningAgentsActionObjectElement& InActionObjectElement, const int32 AgentId) override;
	virtual void MakeAgentActionModifier_Implementation(FLearningAgentsActionModifierElement& OutActionModifierElement, 
														ULearningAgentsActionModifier* InActionModifier, const ULearningAgentsObservationObject* InObservationObject,
														const FLearningAgentsObservationObjectElement& InObservationObjectElement, const int32 AgentId) override;
protected:
	// Use these to control what observations are made by this interactor  
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer RelevantObservations;
	
	// Use these to control what actions are made by this interactor  
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer RelevantActions;
	
	bool bImitationRecordingMode = false;
};

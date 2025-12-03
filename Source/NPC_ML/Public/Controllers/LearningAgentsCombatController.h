// 

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsController.h"
#include "Data/ImitationLearningDataTypes.h"
#include "LearningAgentsCombatController.generated.h"

UCLASS()
class NPC_ML_API ULearningAgentsCombatController : public ULearningAgentsController
{
	GENERATED_BODY()

public:
	virtual void EvaluateAgentController_Implementation(FLearningAgentsActionObjectElement& OutActionObjectElement,
		ULearningAgentsActionObject* InActionObject, const ULearningAgentsObservationObject* InObservationObject, 
		const FLearningAgentsObservationObjectElement& InObservationObjectElement, const int32 AgentId) override;

	// хуйня какая то. надо по человечески сделать
	FORCEINLINE TMap<int, LearningAgentsImitationActions::FAgentPendingActions>& GetPendingActionQueues() { return PendingActionQueues; }
	
private:
	// key = agent id
	TMap<int, LearningAgentsImitationActions::FAgentPendingActions> PendingActionQueues;
};

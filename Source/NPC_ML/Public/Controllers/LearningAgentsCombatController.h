// 

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsController.h"
#include "LearningAgentsCombatController.generated.h"

/**
 * 
 */
UCLASS()
class NPC_ML_API ULearningAgentsCombatController : public ULearningAgentsController
{
	GENERATED_BODY()

public:
	virtual void EvaluateAgentController_Implementation(FLearningAgentsActionObjectElement& OutActionObjectElement, ULearningAgentsActionObject* InActionObject, const ULearningAgentsObservationObject* InObservationObject, const FLearningAgentsObservationObjectElement& InObservationObjectElement, const int32 AgentId) override;
};

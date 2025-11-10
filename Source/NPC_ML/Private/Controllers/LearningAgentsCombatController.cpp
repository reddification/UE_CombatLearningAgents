// 


#include "Controllers/LearningAgentsCombatController.h"

void ULearningAgentsCombatController::EvaluateAgentController_Implementation(
	FLearningAgentsActionObjectElement& OutActionObjectElement, ULearningAgentsActionObject* InActionObject,
	const ULearningAgentsObservationObject* InObservationObject,
	const FLearningAgentsObservationObjectElement& InObservationObjectElement, const int32 AgentId)
{
	Super::EvaluateAgentController_Implementation(OutActionObjectElement, InActionObject, InObservationObject,
	                                              InObservationObjectElement, AgentId);
}

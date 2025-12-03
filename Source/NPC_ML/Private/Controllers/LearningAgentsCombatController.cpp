// 


#include "Controllers/LearningAgentsCombatController.h"

#include "LearningAgentsInteractor.h"
#include "Data/ImitationLearningDataTypes.h"
#include "Data/LearningAgentsSchemaKeys.h"

using namespace LAActionKeys;

void ULearningAgentsCombatController::EvaluateAgentController_Implementation(
	FLearningAgentsActionObjectElement& OutActionObjectElement, ULearningAgentsActionObject* InActionObject,
	const ULearningAgentsObservationObject* InObservationObject,
	const FLearningAgentsObservationObjectElement& InObservationObjectElement, const int32 AgentId)
{
	Super::EvaluateAgentController_Implementation(OutActionObjectElement, InActionObject, InObservationObject,
	                                              InObservationObjectElement, AgentId);

	auto AgentActor = Cast<AActor>(GetAgent(AgentId));
	if (auto AgentActionsQueue = PendingActionQueues.Find(AgentId))
		if (auto PendingAction = AgentActionsQueue->GetAction())
			OutActionObjectElement = PendingAction->GetAction(InActionObject, AgentActor);

	if (OutActionObjectElement.ObjectElement.Index == INDEX_NONE)
	{
		auto NullElement = ULearningAgentsActions::MakeNullAction(InActionObject, Key_Action_Null);
		auto Action = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, Key_Action_Null, NullElement, Key_Action_All);
		OutActionObjectElement = Action;
	}
}

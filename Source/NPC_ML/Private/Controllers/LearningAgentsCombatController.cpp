#include "Controllers/LearningAgentsCombatController.h"

#include "LearningAgentsInteractor.h"
#include "LearningAction.h"
#include "Data/LearningAgentsSchemaKeys.h"
#include "LearningEntities/Interactors/LearningAgentsInteractor_Combat.h"

using namespace LAActionKeys;

void ULearningAgentsCombatController::EvaluateAgentController_Implementation(
	FLearningAgentsActionObjectElement& OutActionObjectElement, ULearningAgentsActionObject* InActionObject,
	const ULearningAgentsObservationObject* InObservationObject,
	const FLearningAgentsObservationObjectElement& InObservationObjectElement, const int32 AgentId)
{
	// 1 Mar 2026 (aki): calling Super produces an error in logs which is wrong IMO. It interferes with my debugging process so I'm commenting it until plugin works properly
	// TODO uncomment super ASAP
	// Super::EvaluateAgentController_Implementation(OutActionObjectElement, InActionObject, InObservationObject, InObservationObjectElement, AgentId);

	auto AgentActor = Cast<AActor>(GetAgent(AgentId));
	if (auto AgentActionsQueue = PendingActionBuffers.Find(AgentId))
		if (auto PendingAction = AgentActionsQueue->GetAction())
			OutActionObjectElement = PendingAction->GetAction(InActionObject, AgentActor, Cast<ULearningAgentsInteractor_Combat>(Interactor.Get()));

	if (OutActionObjectElement.ObjectElement.Index == INDEX_NONE)
	{
		auto NullElement = ULearningAgentsActions::MakeNullAction(InActionObject, Key_Action_Null);
		auto Action = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, Key_Action_Null, NullElement, Key_Action_All);
		OutActionObjectElement = Action;
	}
}

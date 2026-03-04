#include "Data/ILActions/ILAction_Dodge.h"

using namespace LAActionKeys;

namespace LearningAgentsImitationActions
{
	FLearningAgentsActionObjectElement FAction_Dodge::GetAction(ULearningAgentsActionObject* InActionObject,
	    AActor* AgentActor, ULearningAgentsInteractor_Combat* Interactor) const
	{
		auto Action = ULearningAgentsActions::MakeDirectionAction(InActionObject, DodgeDirectionWorld.GetSafeNormal2D(), AgentActor->GetActorTransform(), GetActionName());
		auto CombatExclusiveUnionAction = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, GetActionName(), 
			Action, Key_Action_Combat);
		
		return ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, Key_Action_Combat, CombatExclusiveUnionAction,
			Key_Action_All);
	}
}
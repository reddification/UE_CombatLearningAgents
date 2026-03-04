#include "Data/ILActions/ILAction_Parry.h"

namespace LearningAgentsImitationActions
{
	FLearningAgentsActionObjectElement FAction_Parry::GetAction(ULearningAgentsActionObject* InActionObject,
	    AActor* AgentActor, ULearningAgentsInteractor_Combat* Interactor) const
	{
		auto Action = ULearningAgentsActions::MakeNullAction(InActionObject, GetActionName());
		auto CombatExclusiveUnionAction = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, GetActionName(), 
			Action, LAActionKeys::Key_Action_Combat);
		
		return ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, LAActionKeys::Key_Action_Combat, CombatExclusiveUnionAction,
		LAActionKeys::Key_Action_All);
	}
}

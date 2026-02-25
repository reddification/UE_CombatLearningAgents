#include "Data/ILActions/ILAction_Attack.h"

namespace LearningAgentsImitationActions
{
	FLearningAgentsActionObjectElement FAction_Attack::GetAction(ULearningAgentsActionObject* InActionObject,
																 AActor* AgentActor) const
	{
		auto Action = ULearningAgentsActions::MakeEnumAction(InActionObject, AttackEnum.Get(), AttackIndex, GetActionName());
		auto CombatExclusiveUnionAction = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, GetActionName(), 
			Action, LAActionKeys::Key_Action_Combat);
		
		return ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, LAActionKeys::Key_Action_Combat, CombatExclusiveUnionAction,
		    LAActionKeys::Key_Action_All);
	}
}
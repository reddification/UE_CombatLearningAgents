#include "Data/ILActions/ILAction_UseConsumableItem.h"

namespace LearningAgentsImitationActions
{
	bool FAction_UseConsumableItem::CanCombine(FAction* OtherAction) const
	{
		auto Base = Super::CanCombine(OtherAction);
		if (!Base)
			return false;
		
		return !IsTypeOf(OtherAction->GetType());
	}

	FLearningAgentsActionObjectElement FAction_UseConsumableItem::GetAnimationAction(
		ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const
	{
		return ULearningAgentsActions::MakeNamedExclusiveDiscreteAction(InActionObject, ItemTag.GetTagName(), LAActionKeys::Key_Action_UseConsumableItem);
	}
}
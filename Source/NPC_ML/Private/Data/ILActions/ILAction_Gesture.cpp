// 


#include "Data/ILActions/ILAction_Gesture.h"

namespace LearningAgentsImitationActions
{
	void FAction_Gesture::Combine(const TSharedPtr<FAction>& OtherAction)
	{
		Super::Combine(OtherAction);
		if (OtherAction->IsTypeOf(GetType()))
			GestureTag = StaticCastSharedPtr<FAction_Gesture>(OtherAction)->GestureTag;
	}

	FLearningAgentsActionObjectElement FAction_Gesture::GetAnimationAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const
	{
		return ULearningAgentsActions::MakeNamedExclusiveDiscreteAction(InActionObject, GestureTag.GetTagName(), LAActionKeys::Key_Action_Gesture);
	}
}
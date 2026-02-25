#include "Data/ILActions/ILAction_AnimationBase.h"

namespace LearningAgentsImitationActions
{
	bool FAction_AnimationBase::CanCombine(FAction* OtherAction) const
	{
		return Super::CanCombine(OtherAction) && !OtherAction->IsChildOf(Key_Action_Locomotion_NonBlocking_Animation);
	}

	FLearningAgentsActionObjectElement FAction_AnimationBase::GetActionInternal(
		ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const
	{
		return ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, GetAnimationActionName(),
			GetAnimationAction(InActionObject, AgentActor), Key_Action_Locomotion_NonBlocking_Animation);
	}
}
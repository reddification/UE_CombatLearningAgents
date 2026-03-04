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
		auto Action = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, GetAnimationActionName(),
			GetAnimationAction(InActionObject, AgentActor), Key_Action_Locomotion_NonBlocking_Animation);
		auto ActionOptional = ULearningAgentsActions::MakeOptionalValidAction(InActionObject, Action, Key_Action_Locomotion_NonBlocking_OptionalElement);
		return ActionOptional;
	}
}
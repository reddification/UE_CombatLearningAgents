#include "Data/ILActions/ILAction_Locomotion_Move.h"

#include "LearningAgentsActions.h"
#include "Data/ILActions/ILAction_Base.h"
#include "Data/ILActions/ILAction_Locomotion_NonBlocking.h"

using namespace LAActionKeys;

namespace LearningAgentsImitationActions
{
	FLearningAgentsActionObjectElement FAction_Locomotion_Move::GetActionInternal(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const
	{
		// auto Transform = FTransform::Identity; 
		auto Transform = AgentActor->GetActorTransform();
		// i think it should be identity and not agent's transform, because direction here is an input and input is already relative to the agent
		auto Action = ULearningAgentsActions::MakeDirectionAction(InActionObject, DirectionWorld, Transform, GetActionName());
		auto ActionOptional = ULearningAgentsActions::MakeOptionalValidAction(InActionObject, Action, Key_Action_Locomotion_NonBlocking_OptionalElement);
		return ActionOptional;
	}
	
	bool FAction_Locomotion_Move::CanCombine(FAction* OtherAction) const
	{
		bool bMergeBase = Super::CanCombine(OtherAction);
		if (!bMergeBase)
			return false;
		
		if (IsTypeOf(OtherAction->GetType()))
		{
			auto OtherActionMove = static_cast<FAction_Locomotion_Move*>(OtherAction);
			constexpr float MergeDotProductThreshold = 0.98f;
			return (DirectionWorld | OtherActionMove->DirectionWorld) > MergeDotProductThreshold;
		}
		
		return true;
	}

	void FAction_Locomotion_Move::Combine(const TSharedPtr<FAction>& OtherAction)
	{
		Super::Combine(OtherAction);
		if (IsTypeOf(OtherAction->GetType()))
		{
			auto OtherActionMove = static_cast<FAction_Locomotion_Move*>(OtherAction.Get());
			DirectionWorld = (DirectionWorld + OtherActionMove->DirectionWorld).GetSafeNormal();
		}
	}
}

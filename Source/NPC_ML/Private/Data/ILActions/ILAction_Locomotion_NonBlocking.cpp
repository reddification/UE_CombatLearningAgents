#include "Data/ILActions/ILAction_Locomotion_NonBlocking.h"

#include "Data/LearningAgentsSchemaKeys.h"

using namespace LAActionKeys;

namespace LearningAgentsImitationActions
{
	FAction_Locomotion_NonBlocking::~FAction_Locomotion_NonBlocking()
	{
		PendingSimultaneousActions.Reset();		
	}

	FLearningAgentsActionObjectElement FAction_Locomotion_NonBlocking::GetAction(ULearningAgentsActionObject* InActionObject,
		AActor* AgentActor) const
	{
		TMap<FName, FLearningAgentsActionObjectElement> Map;
		Map.Reserve(1 + PendingSimultaneousActions.Num());
		Map.Add(GetType(), GetActionInternal(InActionObject, AgentActor));
		for (const auto& PendingAction : PendingSimultaneousActions)
		{
			auto LocomotionAction = StaticCastSharedPtr<FAction_Locomotion_NonBlocking>(PendingAction.Value);
			Map.Add(PendingAction.Key, LocomotionAction->GetActionInternal(InActionObject, AgentActor));
		}
		
		auto InclusiveLocomotionActions = ULearningAgentsActions::MakeInclusiveUnionAction(InActionObject,
			Map, LAActionKeys::Key_Action_Locomotion_NonBlocking);
		auto LocomotionActions = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject,
		    Key_Action_Locomotion_NonBlocking, InclusiveLocomotionActions);
		auto LocomotionExclusiveUnionAction = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, Key_Action_Locomotion, 
			LocomotionActions, Key_Action_Locomotion);
		
		return ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, Key_Action_Locomotion, LocomotionExclusiveUnionAction,
			Key_Action_All);
	}

	bool FAction_Locomotion_NonBlocking::CanCombine(FAction* OtherAction) const
	{
		auto Base = Super::CanCombine(OtherAction);
		if (!Base)
			return false;
		
		if (!IsChildOf(Key_Action_Locomotion_NonBlocking))
			return false;
		
		if (auto AlreadyAddedAction = PendingSimultaneousActions.Find(OtherAction->GetActionName()))
			return (*AlreadyAddedAction)->CanCombine(OtherAction);
		
		return true;
	}

	void FAction_Locomotion_NonBlocking::Combine(const TSharedPtr<FAction>& OtherAction)
	{
		const auto& OtherActionType = OtherAction->GetType();
		if (!IsTypeOf(OtherActionType))
		{
			if (auto AlreadyAddedAction = PendingSimultaneousActions.Find(OtherActionType))
				(*AlreadyAddedAction)->Combine(OtherAction);
			else 
				PendingSimultaneousActions.Add(OtherActionType, OtherAction);
		}
	}
}
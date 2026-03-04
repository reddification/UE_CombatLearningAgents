#include "Data/ILActions/ILAction_Locomotion_NonBlocking.h"

#include "Data/LearningAgentsSchemaKeys.h"
#include "LearningEntities/Interactors/LearningAgentsInteractor_Combat.h"

using namespace LAActionKeys;

namespace LearningAgentsImitationActions
{
	FAction_Locomotion_NonBlocking::~FAction_Locomotion_NonBlocking()
	{
		PendingSimultaneousActions.Reset();		
	}

	FLearningAgentsActionObjectElement FAction_Locomotion_NonBlocking::GetAction(ULearningAgentsActionObject* InActionObject,
		AActor* AgentActor, ULearningAgentsInteractor_Combat* Interactor) const
	{
		TMap<FName, FLearningAgentsActionObjectElement> Map;
		// Map.Reserve(AllInclusiveActions.Num());
		Map.Reserve(1 + PendingSimultaneousActions.Num());
		Map.Add(GetActionName(), GetActionInternal(InActionObject, AgentActor));
		for (const auto& PendingAction : PendingSimultaneousActions)
		{
			auto LocomotionAction = StaticCastSharedPtr<FAction_Locomotion_NonBlocking>(PendingAction.Value);
			Map.Add(PendingAction.Key, LocomotionAction->GetActionInternal(InActionObject, AgentActor));
		}
		
		const TArray<FName>& AllInclusiveActions = Interactor->GetLocomotionNonBlockingActionNames();
		for (const auto& RequiredAction : AllInclusiveActions)
		{
			if (!Map.Contains(RequiredAction))
			{
				// FName OptionalNullName = FName(FString::Printf(TEXT("%s.Optional"), *RequiredAction.ToString()));
				auto OptionalNull = ULearningAgentsActions::MakeOptionalNullAction(InActionObject, Key_Action_Locomotion_NonBlocking_OptionalElement);
				Map.Add(RequiredAction, OptionalNull);
			}
		}
		
		auto InclusiveLocomotionActions = ULearningAgentsActions::MakeStructAction(InActionObject, Map, 
			Key_Action_Locomotion_NonBlocking);
		// auto InclusiveLocomotionActions = ULearningAgentsActions::MakeInclusiveUnionAction(InActionObject,
		// 	Map, Key_Action_Locomotion_NonBlocking);
		auto LocomotionExclusiveUnionAction = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, Key_Action_Locomotion_NonBlocking, 
			InclusiveLocomotionActions, Key_Action_Locomotion);
		
		return ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, Key_Action_Locomotion, LocomotionExclusiveUnionAction,
			Key_Action_All);
	}

	bool FAction_Locomotion_NonBlocking::CanCombine(FAction* OtherAction) const
	{
		auto Base = Super::CanCombine(OtherAction);
		if (!Base)
			return false;
		
		if (!OtherAction->IsChildOf(Key_Action_Locomotion_NonBlocking))
			return false;
		
		if (auto AlreadyAddedAction = PendingSimultaneousActions.Find(OtherAction->GetActionName()))
			return (*AlreadyAddedAction)->CanCombine(OtherAction);
		
		return true;
	}

	void FAction_Locomotion_NonBlocking::Combine(const TSharedPtr<FAction>& OtherAction)
	{
		Super::Combine(OtherAction);
		const auto& OtherActionType = OtherAction->GetType();
		if (!IsTypeOf(OtherActionType))
		{
			if (auto AlreadyAddedAction = PendingSimultaneousActions.Find(OtherActionType))
				(*AlreadyAddedAction)->Combine(OtherAction);
			else 
				PendingSimultaneousActions.Add(OtherAction->GetActionName(), OtherAction);
		}
	}

	// TArray<FName> FAction_Locomotion_NonBlocking::GetAllPossibleInclusiveActionsNames() const
	// {
	// 	// 3 Mar 2026 (aki): . 
	// 	// IIUC, inclusive action still requires for all subactions to present in sampled or made by imitator actions
	// 	// but, IIUC, null action can be provided when there is no real subaction
	// 	// so the plan right now is to: if action present - provide it into union. if action is not present - put NULL action
	// 	// the problem here is that the real authority of what subactions inclusive union consists of (and hence what subactions must always be provided)
	// 	// is the schema. I am now hardcoding these action keys here but this is wrong
	// 	// TODO implement a proper retreival of inclusive union subactions to substitue missing actions with NULL-dummies
	// 	return 
	//  {
	//  		Key_Action_Locomotion_Rotate,
	//  		Key_Action_Locomotion_NonBlocking_Animation,
	//  		Key_Action_Locomotion_SetSpeed,
	//  		Key_Action_Locomotion_Move
	// 	};
	// }
}

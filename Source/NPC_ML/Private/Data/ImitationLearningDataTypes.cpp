#include "Data/ImitationLearningDataTypes.h"

#include "Data/LearningAgentsSchemaKeys.h"
#include "Settings/CombatLearningSettings.h"

using namespace LAActionKeys;

namespace LearningAgentsImitationActions
{
	bool FAction::CanCombine(FAction* OtherAction) const
	{
		constexpr float MergeDeltaTimeThreshold = 0.01f;
		return OtherAction->Timestamp - Timestamp < MergeDeltaTimeThreshold;
	}

	FAction_Locomotion_NonBlocking::~FAction_Locomotion_NonBlocking()
	{
		PendingSimultaneousActions.Reset();		
	}

	FLearningAgentsActionObjectElement FAction_Locomotion_NonBlocking::GetAction(
		ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const
	{
		TMap<FName, FLearningAgentsActionObjectElement> Map;
		Map.Reserve(1 + PendingSimultaneousActions.Num());
		Map.Add(GetActionName(), GetActionInternal(InActionObject, AgentActor));
		for (const auto& PendingAction : PendingSimultaneousActions)
		{
			auto LocomotionAction = StaticCastSharedPtr<FAction_Locomotion_NonBlocking>(PendingAction.Value);
			Map.Add(PendingAction.Key, LocomotionAction->GetActionInternal(InActionObject, AgentActor));
		}
		
		auto InclusiveLocomotionActions = ULearningAgentsActions::MakeInclusiveUnionAction(InActionObject,
			Map, Key_Action_Locomotion_NonBlocking);
		auto LocomotionActions = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject,
			Key_Action_Locomotion_NonBlocking, InclusiveLocomotionActions);
		auto LocomotionExclusiveUnionAction = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, Key_Actions_Locomotion, 
			LocomotionActions, Key_Actions_Locomotion);
		
		return ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, Key_Actions_Locomotion, LocomotionExclusiveUnionAction,
			Key_Action_All);
	}

	bool FAction_Locomotion_NonBlocking::CanCombine(FAction* OtherAction) const
	{
		auto Base = FAction::CanCombine(OtherAction);
		if (!Base)
			return false;
		
		if (GetActionName() == OtherAction->GetActionName())
			return true;
		
		// yeah yeah fuck RTTI and shit
		// FAction_Locomotion_NonBlocking* LocomotionAction = dynamic_cast<FAction_Locomotion_NonBlocking*>(OtherAction);
		// if (LocomotionAction == nullptr)
			// return false;
		
		const TSet<FName> ValidActions = 
		{
			Key_Action_Locomotion_Move,
			Key_Action_Locomotion_Rotate,
			Key_Action_Locomotion_SetSpeed,
		};
		
		if (!ValidActions.Contains(OtherAction->GetActionName()))
			return false;
		
		if (auto AlreadyAddedAction = PendingSimultaneousActions.Find(OtherAction->GetActionName()))
			return (*AlreadyAddedAction)->CanCombine(OtherAction);
		
		return true;
	}

	void FAction_Locomotion_NonBlocking::Combine(const TSharedPtr<FAction>& OtherAction)
	{
		FAction::Combine(OtherAction);
		auto OtherActionName = OtherAction->GetActionName();
		if (OtherActionName != GetActionName())
		{
			if (auto AlreadyAddedAction = PendingSimultaneousActions.Find(OtherActionName))
				(*AlreadyAddedAction)->Combine(OtherAction);
			else 
				PendingSimultaneousActions.Add(OtherActionName, OtherAction);
		}
	}

	FLearningAgentsActionObjectElement FAction_Locomotion_Move::GetActionInternal(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const
	{
		auto Transform = FTransform::Identity; // AgentActor->GetActorTransform()
		// i think it should be identity and not agent's transform, because direction here is an input and input is already relative to the agent
		return ULearningAgentsActions::MakeDirectionAction(InActionObject, Direction, Transform, GetActionName());
	}

	bool FAction_Locomotion_Move::CanCombine(FAction* OtherAction) const
	{
		bool bMergeBase = FAction_Locomotion_NonBlocking::CanCombine(OtherAction);
		if (!bMergeBase)
			return false;
		
		if (OtherAction->GetActionName() != GetActionName())
			return false;
		
		auto OtherActionMove = static_cast<FAction_Locomotion_Move*>(OtherAction);
		constexpr float MergeDotProductThreshold = 0.98f;
		return (Direction | OtherActionMove->Direction) > MergeDotProductThreshold;
	}

	void FAction_Locomotion_Move::Combine(const TSharedPtr<FAction>& OtherAction)
	{
		FAction_Locomotion_NonBlocking::Combine(OtherAction);
		if (OtherAction->GetActionName() != GetActionName())
		{
			ensure(false);
			return;
		}
		
		auto OtherActionMove = static_cast<FAction_Locomotion_Move*>(OtherAction.Get());
		Direction = (Direction + OtherActionMove->Direction).GetSafeNormal();
	}

	FLearningAgentsActionObjectElement FAction_Locomotion_SetSpeed::GetActionInternal(
		ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const
	{
		return ULearningAgentsActions::MakeFloatAction(InActionObject, Speed, GetActionName());
	}

	bool FAction_Locomotion_SetSpeed::CanCombine(FAction* OtherAction) const
	{
		auto Base = FAction_Locomotion_NonBlocking::CanCombine(OtherAction);
		if (!Base)
			return false;
		
		if (OtherAction->GetActionName() != GetActionName())
			return false;
		
		auto OtherSetSpeedAction = static_cast<FAction_Locomotion_SetSpeed*>(OtherAction);
		constexpr float MinSpeedDelta = 10.f;
		return FMath::Abs(Speed - OtherSetSpeedAction->Speed) < MinSpeedDelta;
	}

	void FAction_Locomotion_SetSpeed::Combine(const TSharedPtr<FAction>& OtherAction)
	{
		FAction_Locomotion_NonBlocking::Combine(OtherAction);
		if (OtherAction->GetActionName() != GetActionName())
			return;
		
		auto OtherActionSetSpeed = static_cast<FAction_Locomotion_SetSpeed*>(OtherAction.Get());
		Speed = OtherActionSetSpeed->Speed;
	}

	FLearningAgentsActionObjectElement FAction_Locomotion_Rotate::GetActionInternal(
		ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const
	{
		return ULearningAgentsActions::MakeRotationAction(InActionObject, Rotator, FRotator::ZeroRotator, GetActionName());
	}

	FLearningAgentsActionObjectElement FAction_Locomotion_Jump::GetAction(
		ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const
	{
		auto Action = ULearningAgentsActions::MakeDirectionAction(InActionObject, Direction, FTransform::Identity, GetActionName());
		auto LocomotionExclusiveUnionAction = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, GetActionName(), 
			Action, Key_Actions_Locomotion);
		
		return ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, Key_Actions_Locomotion, LocomotionExclusiveUnionAction,
			Key_Action_All);
	}

	FLearningAgentsActionObjectElement FAction_Locomotion_Climb::GetAction(
		ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const
	{
		auto Action = ULearningAgentsActions::MakeDirectionAction(InActionObject, Direction, FTransform::Identity, GetActionName());
		auto LocomotionExclusiveUnionAction = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, GetActionName(), 
			Action, Key_Actions_Locomotion);
		
		return ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, Key_Actions_Locomotion, LocomotionExclusiveUnionAction,
			Key_Action_All);
	}

	FLearningAgentsActionObjectElement FAction_Locomotion_Gesture::GetActionInternal(
		ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const
	{
		return ULearningAgentsActions::MakeNamedExclusiveDiscreteAction(InActionObject, GestureTag.GetTagName(), GetActionName());
	}

	FLearningAgentsActionObjectElement FAction_Locomotion_Phrase::GetActionInternal(
		ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const
	{
		return ULearningAgentsActions::MakeNamedExclusiveDiscreteAction(InActionObject, PhraseTag.GetTagName(), GetActionName());
	}

	FLearningAgentsActionObjectElement FAction_Locomotion_UseConsumableItem::GetActionInternal(
		ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const
	{
		return ULearningAgentsActions::MakeNamedExclusiveDiscreteAction(InActionObject, ItemTag.GetTagName(), GetActionName());
	}

	FLearningAgentsActionObjectElement FAction_Attack::GetAction(ULearningAgentsActionObject* InActionObject,
		AActor* AgentActor) const
	{
		auto Action = ULearningAgentsActions::MakeExclusiveDiscreteAction(InActionObject, AttackIndex, GetActionName());
		auto CombatExclusiveUnionAction = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, GetActionName(), 
			Action, Key_Action_Combat);
		
		return ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, Key_Action_Combat, CombatExclusiveUnionAction,
			Key_Action_All);
	}

	FLearningAgentsActionObjectElement FAction_Parry::GetAction(ULearningAgentsActionObject* InActionObject,
		AActor* AgentActor) const
	{
		auto Action = ULearningAgentsActions::MakeDirectionAction(InActionObject, ParryDirection, FTransform::Identity, GetActionName());
		auto CombatExclusiveUnionAction = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, GetActionName(), 
			Action, Key_Action_Combat);
		
		return ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, Key_Action_Combat, CombatExclusiveUnionAction,
			Key_Action_All);
	}

	FLearningAgentsActionObjectElement FAction_Dodge::GetAction(ULearningAgentsActionObject* InActionObject,
		AActor* AgentActor) const
	{
		auto Action = ULearningAgentsActions::MakeDirectionAction(InActionObject, DodgeDirection, FTransform::Identity, GetActionName());
		auto CombatExclusiveUnionAction = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, GetActionName(), 
			Action, Key_Action_Combat);
		
		return ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, Key_Action_Combat, CombatExclusiveUnionAction,
			Key_Action_All);
	}

	FAgentPendingActions::FAgentPendingActions(int InAgentId)
	{
		AgentId = InAgentId;
	}

	FAgentPendingActions::~FAgentPendingActions()
	{
		AccumulatingAction.Reset();
		PendingAction.Reset();
	}

	void FAgentPendingActions::AddAction(const TSharedPtr<FAction>& NewAction)
	{
		bool bActionImmediate = NewAction->IsImmediate();
		if (!AccumulatingAction.IsValid())
		{
			AccumulatingAction = NewAction;
		}
		else
		{
			if (AccumulatingAction->CanCombine(NewAction.Get()))
			{
				AccumulatingAction->Combine(NewAction);				
			}
			else
			{
				if (!ensure(!PendingAction.IsValid()))
					PendingAction.Reset();
				
				PendingAction = NewAction;
				ActionAccumulatedEvent.ExecuteIfBound(AgentId);
			}
		}
		
		if (bActionImmediate)
			ActionAccumulatedEvent.ExecuteIfBound(AgentId);
	}

	TSharedPtr<FAction> FAgentPendingActions::GetAction()
	{
		bool bGotAction = AccumulatingAction.IsValid();
		TSharedPtr<FAction> Result = ensure(bGotAction) ? AccumulatingAction : nullptr;
		AccumulatingAction.Reset();
		if (PendingAction.IsValid())
		{
			AccumulatingAction = PendingAction;
			PendingAction.Reset();
		}
		
		return Result;
		
	}
}

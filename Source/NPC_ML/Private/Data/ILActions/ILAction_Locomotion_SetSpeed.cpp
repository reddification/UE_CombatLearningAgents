#include "Data/ILActions/ILAction_Locomotion_SetSpeed.h"
#include "LearningAgentsActions.h"

namespace LearningAgentsImitationActions
{
	FLearningAgentsActionObjectElement FAction_Locomotion_SetSpeed::GetActionInternal(
		ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const
	{
		return ULearningAgentsActions::MakeFloatAction(InActionObject, Speed, GetActionName());
	}

	bool FAction_Locomotion_SetSpeed::CanCombine(FAction* OtherAction) const
	{
		auto Base = Super::CanCombine(OtherAction);
		if (!Base)
			return false;
		
		if (!IsTypeOf(OtherAction->GetType()))
			return true;
		
		auto OtherSetSpeedAction = static_cast<FAction_Locomotion_SetSpeed*>(OtherAction);
		constexpr float MinSpeedDelta = 10.f;
		return FMath::Abs(Speed - OtherSetSpeedAction->Speed) < MinSpeedDelta;
	}

	void FAction_Locomotion_SetSpeed::Combine(const TSharedPtr<FAction>& OtherAction)
	{
		Super::Combine(OtherAction);
		if (IsTypeOf(OtherAction->GetType()))
		{
			auto OtherActionSetSpeed = static_cast<FAction_Locomotion_SetSpeed*>(OtherAction.Get());
			Speed = OtherActionSetSpeed->Speed;
		}		
	}
}

#include "Data/ILActions/ILAction_Locomotion_Rotate.h"

namespace LearningAgentsImitationActions
{
	bool FAction_Locomotion_Rotate::CanCombine(FAction* OtherAction) const
	{
		auto Base = Super::CanCombine(OtherAction);
		if (!Base)
			return false;
		
		if (!IsTypeOf(OtherAction->GetType()))
			return true;
		
		auto OtherRotationInput = static_cast<FAction_Locomotion_Rotate*>(OtherAction)->BaseRotationInput;
		float constexpr DotProductThreshold = 0.9;
		return (BaseRotationInput.Vector() | OtherRotationInput.Vector()) >= DotProductThreshold;
	}

	void FAction_Locomotion_Rotate::Combine(const TSharedPtr<FAction>& OtherAction)
	{
		Super::Combine(OtherAction);
		if (IsTypeOf(OtherAction->GetType()))
		{
			AccumulatedRotationInput += StaticCastSharedPtr<FAction_Locomotion_Rotate>(OtherAction)->BaseRotationInput;
		}
	}

	FLearningAgentsActionObjectElement FAction_Locomotion_Rotate::GetActionInternal(ULearningAgentsActionObject* InActionObject,
		AActor* AgentActor) const
	{
		auto Action = ULearningAgentsActions::MakeRotationAction(InActionObject, AccumulatedRotationInput,
			FRotator::ZeroRotator, GetActionName());
		auto ActionOptional = ULearningAgentsActions::MakeOptionalValidAction(InActionObject, Action, LAActionKeys::Key_Action_Locomotion_NonBlocking_OptionalElement);
		return ActionOptional;
	}
}
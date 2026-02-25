#include "Data/ILActions/ILAction_ChangeWeaponState.h"

namespace LearningAgentsImitationActions
{
	FLearningAgentsActionObjectElement FAction_ChangeWeaponState::GetAnimationAction(
		ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const
	{
		return ULearningAgentsActions::MakeEnumAction(InActionObject, StaticEnum<ELAWeaponStateChange>(), static_cast<uint8>(WeaponState),
		Key_Action_ChangeWeaponState);
	}

	void FAction_ChangeWeaponState::Combine(const TSharedPtr<FAction>& OtherAction)
	{
		Super::Combine(OtherAction);
		if (OtherAction->IsTypeOf(GetType()))
			WeaponState = StaticCastSharedPtr<FAction_ChangeWeaponState>(OtherAction)->WeaponState;
	}
}

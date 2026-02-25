// 

#pragma once

#include "CoreMinimal.h"
#include "ILAction_AnimationBase.h"
#include "Data/LearningAgentsDataTypes.h"

namespace LearningAgentsImitationActions
{
	class FAction_ChangeWeaponState : public FAction_AnimationBase
	{
	private: 
		using Super = FAction_AnimationBase;
		
	public:
		FAction_ChangeWeaponState(float Timestamp, ELAWeaponStateChange NewWeaponState)
			: Super(Timestamp), WeaponState(NewWeaponState)
		{
			SetType(Key_Action_ChangeWeaponState);
		}
						
		virtual FName GetAnimationActionName() const override { return Key_Action_ChangeWeaponState; }
		virtual FLearningAgentsActionObjectElement GetAnimationAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
		virtual void Combine(const TSharedPtr<FAction>& OtherAction) override;
		
	private:
		ELAWeaponStateChange WeaponState;
	};
}

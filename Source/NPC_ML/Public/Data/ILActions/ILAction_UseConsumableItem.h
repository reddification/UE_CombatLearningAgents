// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ILAction_AnimationBase.h"

namespace LearningAgentsImitationActions
{
	class FAction_UseConsumableItem : public FAction_AnimationBase
	{
		using Super = FAction_AnimationBase;
	public:
		FAction_UseConsumableItem(float Timestamp, const FGameplayTag& ItemTag) : Super(Timestamp), ItemTag(ItemTag)
		{
			SetType(Key_Action_UseConsumableItem);
		}
					
		virtual bool CanCombine(FAction* OtherAction) const override;
		virtual FName GetAnimationActionName() const override { return Key_Action_UseConsumableItem; }
		virtual FLearningAgentsActionObjectElement GetAnimationAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
					
	private:
		FGameplayTag ItemTag;
	};
}

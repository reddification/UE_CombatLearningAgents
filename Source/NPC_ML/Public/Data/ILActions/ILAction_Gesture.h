// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ILAction_AnimationBase.h"
#include "LearningAgentsActions.h"
#include "Data/LearningAgentsSchemaKeys.h"

namespace LearningAgentsImitationActions
{
	class NPC_ML_API FAction_Gesture : public FAction_AnimationBase
	{
		using Super = FAction_AnimationBase;
		
	public:
		FAction_Gesture(float Timestamp, const FGameplayTag& InGestureTag) : Super(Timestamp), GestureTag(InGestureTag)
		{
			SetType(Key_Action_Gesture);
		}
					
		virtual void Combine(const TSharedPtr<FAction>& OtherAction) override;
		virtual FName GetAnimationActionName() const override { return Key_Action_Gesture; }
		virtual FLearningAgentsActionObjectElement GetAnimationAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
						
	private:
		FGameplayTag GestureTag;
	};
}

// 

#pragma once

#include "CoreMinimal.h"
#include "ILAction_Locomotion_NonBlocking.h"
#include "Data/LearningAgentsSchemaKeys.h"

using namespace LAActionKeys;

namespace LearningAgentsImitationActions
{
	class FAction_AnimationBase : public FAction_Locomotion_NonBlocking
	{
		
	private:
		using Super = FAction_Locomotion_NonBlocking;
		
	public:
		FAction_AnimationBase(float Timestamp) : Super(Timestamp)
		{
			SetType(Key_Action_Locomotion_NonBlocking_Animation);
		}
		
		virtual bool CanCombine(FAction* OtherAction) const override;
		virtual FName GetActionName() const override final { return Key_Action_Locomotion_NonBlocking_Animation; };
		virtual FName GetAnimationActionName() const = 0;
		virtual FLearningAgentsActionObjectElement GetActionInternal(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
		virtual FLearningAgentsActionObjectElement GetAnimationAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const = 0;
	};
}

// 

#pragma once

#include "CoreMinimal.h"
#include "ILAction_Locomotion_NonBlocking.h"
#include "Data/LearningAgentsSchemaKeys.h"

namespace LearningAgentsImitationActions
{
	class FAction_Locomotion_SetSpeed : public FAction_Locomotion_NonBlocking
	{
		using Super = FAction_Locomotion_NonBlocking;
	public:
		FAction_Locomotion_SetSpeed(float Timestamp, float NewSpeed) : Super(Timestamp), Speed(NewSpeed)
		{
			SetType(LAActionKeys::Key_Action_Locomotion_SetSpeed);
		}
				
		virtual FLearningAgentsActionObjectElement GetActionInternal(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
		virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Locomotion_SetSpeed; }; 
		virtual bool CanCombine(FAction* OtherAction) const override;	
		virtual void Combine(const TSharedPtr<FAction>& OtherAction) override;	
		
	private:
		float Speed = 0.f;
	};
}

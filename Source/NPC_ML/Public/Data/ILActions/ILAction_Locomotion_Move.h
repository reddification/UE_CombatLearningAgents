// 

#pragma once

#include "CoreMinimal.h"
#include "ILAction_Locomotion_NonBlocking.h"
#include "Data/LearningAgentsSchemaKeys.h"

namespace LearningAgentsImitationActions
{
	class FAction_Locomotion_Move : public FAction_Locomotion_NonBlocking
	{
	private:
		using Super = FAction_Locomotion_NonBlocking;
	public:
		FAction_Locomotion_Move(float Timestamp, const FVector& InDirection) : Super(Timestamp), DirectionWorld(InDirection)
		{
			SetType(LAActionKeys::Key_Action_Locomotion_Move);
		}
			
		virtual FLearningAgentsActionObjectElement GetActionInternal(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
		virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Locomotion_Move; }; 
		virtual bool CanCombine(FAction* OtherAction) const override;	
		virtual void Combine(const TSharedPtr<FAction>& OtherAction) override;
		
	private:
		FVector DirectionWorld = FVector::ZeroVector;
	};
	
}

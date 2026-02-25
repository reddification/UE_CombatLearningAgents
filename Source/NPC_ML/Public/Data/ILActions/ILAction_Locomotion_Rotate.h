#pragma once

#include "CoreMinimal.h"
#include "ILAction_Locomotion_NonBlocking.h"
#include "Data/LearningAgentsSchemaKeys.h"

namespace LearningAgentsImitationActions
{
	class FAction_Locomotion_Rotate : public FAction_Locomotion_NonBlocking
	{
		using Super = FAction_Locomotion_NonBlocking;
		
	public:
		FAction_Locomotion_Rotate(float Timestamp, const FRotator& InRotationInput) : Super(Timestamp), 
			BaseRotationInput(InRotationInput), AccumulatedRotationInput(InRotationInput)
		{
			SetType(LAActionKeys::Key_Action_Locomotion_Rotate);
		}

		virtual bool CanCombine(FAction* OtherAction) const override;
		virtual void Combine(const TSharedPtr<FAction>& OtherAction) override;
		virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Locomotion_Rotate; };
		virtual FLearningAgentsActionObjectElement GetActionInternal(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
				
	private:
		FRotator BaseRotationInput = FRotator::ZeroRotator;
		FRotator AccumulatedRotationInput = FRotator::ZeroRotator;
	};
}

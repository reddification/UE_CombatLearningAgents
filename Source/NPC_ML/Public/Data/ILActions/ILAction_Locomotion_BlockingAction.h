#pragma once

#include "CoreMinimal.h"
#include "ILAction_Base.h"
#include "Data/LearningAgentsDataTypes.h"
#include "Data/LearningAgentsSchemaKeys.h"

namespace LearningAgentsImitationActions
{
	class NPC_ML_API FAction_Locomotion_BlockingLocomotion : public FAction
	{
		using Super = FAction;
	public:
		FAction_Locomotion_BlockingLocomotion(float Timestamp, ELALocomotionAction LocomotionAction) 
			: Super(Timestamp), LocomotionAction(LocomotionAction)
		{
			SetType(LAActionKeys::Key_Action_Locomotion_Blocking);
		}
					
		virtual bool IsImmediate() const override { return true; }
		virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Locomotion_Blocking; }
		virtual bool CanCombine(FAction* OtherAction) const override { return false; }
		virtual FLearningAgentsActionObjectElement GetAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
					
	private:
		ELALocomotionAction LocomotionAction;
	};
}

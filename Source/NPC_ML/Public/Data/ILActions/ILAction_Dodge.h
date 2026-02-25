// 

#pragma once

#include "CoreMinimal.h"
#include "ILAction_Base.h"
#include "Data/LearningAgentsSchemaKeys.h"

namespace LearningAgentsImitationActions
{
	class FAction_Dodge : public FAction
	{
		using Super = FAction;
	public:
		FAction_Dodge(float Timestamp, const FVector& InDodgeDirectionWorld)
			: Super(Timestamp), DodgeDirectionWorld(InDodgeDirectionWorld)
		{
			SetType(LAActionKeys::Key_Action_Combat_Dodge);
		}
								
		virtual bool IsImmediate() const override { return true; }
		virtual bool CanCombine(FAction* OtherAction) const override { return false; }
		virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Combat_Dodge; }
		virtual FLearningAgentsActionObjectElement GetAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
								
	private:
		FVector DodgeDirectionWorld = FVector::ZeroVector;
	};
}

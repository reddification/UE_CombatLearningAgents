// 

#pragma once

#include "CoreMinimal.h"
#include "ILAction_Base.h"
#include "Data/LearningAgentsSchemaKeys.h"

namespace LearningAgentsImitationActions
{
	class NPC_ML_API FAction_Parry : public FAction
	{
		using Super = FAction;
		public:
			FAction_Parry(float Timestamp) : Super(Timestamp)
			{
				SetType(LAActionKeys::Key_Action_Combat_Parry);
			}
								
			virtual bool IsImmediate() const override { return true; }
			virtual bool CanCombine(FAction* OtherAction) const override { return false; }
			virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Combat_Parry; }
			virtual FLearningAgentsActionObjectElement GetAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor, ULearningAgentsInteractor_Combat* Interactor) const override;
	};
}

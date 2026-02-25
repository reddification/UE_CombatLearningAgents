#pragma once

#include "CoreMinimal.h"
#include "ILAction_Base.h"
#include "Data/LearningAgentsSchemaKeys.h"

namespace LearningAgentsImitationActions
{
	class FAction_Locomotion_NonBlocking : public FAction
	{
		private:
			using Super = FAction;
		
		public:
			FAction_Locomotion_NonBlocking(float Timestamp) : Super(Timestamp)
			{
				SetType(LAActionKeys::Key_Action_Locomotion_NonBlocking);
			};
		
			virtual ~FAction_Locomotion_NonBlocking() override;
			virtual FLearningAgentsActionObjectElement GetAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
			virtual FLearningAgentsActionObjectElement GetActionInternal(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const = 0;
			virtual bool CanCombine(FAction* OtherAction) const override;
			virtual void Combine(const TSharedPtr<FAction>& OtherAction) override;
			virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Locomotion_NonBlocking; }
		
		private:
			TMap<FName, TSharedPtr<FAction>> PendingSimultaneousActions;
	};	
}

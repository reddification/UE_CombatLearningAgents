#pragma once

#include "CoreMinimal.h"
#include "ILAction_Base.h"
#include "Data/LearningAgentsSchemaKeys.h"

namespace LearningAgentsImitationActions
{
	class NPC_ML_API FAction_Locomotion_NonBlocking : public FAction
	{
		private:
			using Super = FAction;
		
		public:
			FAction_Locomotion_NonBlocking(float Timestamp) : Super(Timestamp)
			{
				SetType(LAActionKeys::Key_Action_Locomotion_NonBlocking);
			};
		
			virtual ~FAction_Locomotion_NonBlocking() override;
			virtual FLearningAgentsActionObjectElement GetAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor, ULearningAgentsInteractor_Combat* Interactor) const override final; 
			virtual FLearningAgentsActionObjectElement GetActionInternal(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const = 0;
			virtual bool CanCombine(FAction* OtherAction) const override;
			virtual void Combine(const TSharedPtr<FAction>& OtherAction) override;
			virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Locomotion_NonBlocking; }
		
		private:
			mutable TMap<FName, TSharedPtr<FAction>> PendingSimultaneousActions;

			// TArray<FName> GetAllPossibleInclusiveActionsNames() const;
		
	};	
}

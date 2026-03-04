#pragma once

#include "LearningAgentsActions.h"

class ULearningAgentsInteractor_Combat;

namespace LearningAgentsImitationActions
{
	class NPC_ML_API FAction
	{
		public:
			FAction(float Timestamp) : Timestamp(Timestamp)
			{
				TypeHierarchy.Add(FName("FAction"));
			};
		
			virtual ~FAction() = default;

			// yeah yeah fuck RTTI and shit
			__forceinline bool IsChildOf(const FName& Type) const { return TypeHierarchy.Contains(Type); }
			__forceinline bool IsTypeOf(const FName& Type) const { return TypeHierarchy.Last() == Type; }
			__forceinline const FName& GetType() const { return TypeHierarchy.Last(); }
					
			// if true, action doesn't wait for other actions to combine
			virtual bool IsImmediate() const { return false; }
		
			virtual bool CanCombine(FAction* OtherAction) const;
			virtual void Combine(const TSharedPtr<FAction>& OtherAction) { }
			
			virtual FLearningAgentsActionObjectElement GetAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor, ULearningAgentsInteractor_Combat* Interactor) const = 0;
			virtual FName GetActionName() const = 0;
		
		protected:
			// 24 Feb 2026 (aki): TODO prevent it from being called anywhere but in constructor
			void SetType(const FName& Type) { TypeHierarchy.Add(Type); }
			float Timestamp = 0.f;
		
		private:
			TArray<FName> TypeHierarchy;
			bool bTypeInitialized = false;
	};
	
	struct NPC_ML_API FAgentPendingActionsBuffer
	{
		DECLARE_DELEGATE_OneParam(FActionAccumulatedEvent, int AgentId);
		
		FAgentPendingActionsBuffer(int AgentId);
		~FAgentPendingActionsBuffer();
		
		void AddAction(const TSharedPtr<FAction>& NewAction);
		TSharedPtr<FAction> GetAction();
	
		FActionAccumulatedEvent ActionAccumulatedEvent;
		
	private:
		// this one is the default action to accumulate to
		TSharedPtr<FAction> AccumulatingAction;
		
		// this is the action where a new action is stored if the AccumulatingAction can't combine new action, which also means that the accumulating action is full 
		TSharedPtr<FAction> PendingAction;
		
		int AgentId = -1;
	};
}

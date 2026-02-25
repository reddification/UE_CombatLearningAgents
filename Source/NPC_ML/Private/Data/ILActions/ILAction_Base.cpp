#include "Data/ILActions/ILAction_Base.h"
#include "Data/LearningAgentsSchemaKeys.h"

using namespace LAActionKeys;

namespace LearningAgentsImitationActions
{
	bool FAction::CanCombine(FAction* OtherAction) const
	{
		constexpr float MergeDeltaTimeThreshold = 0.01f;
		return OtherAction->Timestamp - Timestamp < MergeDeltaTimeThreshold;
	}

	FAgentPendingActionsBuffer::FAgentPendingActionsBuffer(int InAgentId)
	{
		AgentId = InAgentId;
	}

	FAgentPendingActionsBuffer::~FAgentPendingActionsBuffer()
	{
		AccumulatingAction.Reset();
		PendingAction.Reset();
	}

	void FAgentPendingActionsBuffer::AddAction(const TSharedPtr<FAction>& NewAction)
	{
		bool bActionImmediate = NewAction->IsImmediate();
		if (!AccumulatingAction.IsValid())
		{
			AccumulatingAction = NewAction;
		}
		else
		{
			if (AccumulatingAction->CanCombine(NewAction.Get()))
			{
				AccumulatingAction->Combine(NewAction);				
			}
			else
			{
				if (!ensure(!PendingAction.IsValid()))
					PendingAction.Reset();
				
				PendingAction = NewAction;
				ActionAccumulatedEvent.ExecuteIfBound(AgentId);
			}
		}
		
		if (bActionImmediate)
			ActionAccumulatedEvent.ExecuteIfBound(AgentId);
	}

	TSharedPtr<FAction> FAgentPendingActionsBuffer::GetAction()
	{
		bool bGotAction = AccumulatingAction.IsValid();
		TSharedPtr<FAction> Result = ensure(bGotAction) ? AccumulatingAction : nullptr;
		AccumulatingAction.Reset();
		if (PendingAction.IsValid())
		{
			AccumulatingAction = PendingAction;
			PendingAction.Reset();
		}
		
		return Result;
		
	}
}

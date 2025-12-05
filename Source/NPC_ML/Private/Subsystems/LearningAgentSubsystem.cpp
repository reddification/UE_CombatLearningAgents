// 


#include "Subsystems/LearningAgentSubsystem.h"

#include "LearningAgentsManager.h"

void ULearningAgentSubsystem::RegisterLearningAgentsManager(ULearningAgentsManager* NewLearningAgentsManager)
{
	if (LearningAgentsManager.IsValid())
	{
		//TODO move all active agents to new manager
		ensure(false);
	}
	else
	{
		LearningAgentsManager = NewLearningAgentsManager;
		if (PendingAgents.IsEmpty())
			return;

		for (int i = PendingAgents.Num() - 1; i >= 0; --i)
		{
			int AddedIndex = LearningAgentsManager->AddAgent(PendingAgents[i].Get());
			if (AddedIndex == INDEX_NONE)
			{
				ensure(false);
				continue;	
			}
			
			auto AddedGuy = PendingAgents.Pop();
			AddedGuy->AddTickPrerequisiteComponent(NewLearningAgentsManager);
		}
	}
}

void ULearningAgentSubsystem::UnregisterLearningAgentsManager(ULearningAgentsManager* OldLearningAgentsManager)
{
	if (ensure(OldLearningAgentsManager == LearningAgentsManager))
	{
		LearningAgentsManager->RemoveAllAgents();
		LearningAgentsManager.Reset();
	}
}

void ULearningAgentSubsystem::RegisterLearningAgent(APawn* Pawn)
{
	if (LearningAgentsManager.IsValid())
	{
		int RegisteredIndex = LearningAgentsManager->AddAgent(Pawn);
		if (ensure(RegisteredIndex != INDEX_NONE))
			RegisteredAgents.Emplace(Pawn, RegisteredIndex);
	}
	else
	{
		PendingAgents.Push(Pawn);
	}
}

void ULearningAgentSubsystem::UnregisterLearningAgent(APawn* Pawn)
{
	if (LearningAgentsManager.IsValid())
	{
		auto AgentId = LearningAgentsManager->GetAgentId(Pawn);
		if (ensure(AgentId != INDEX_NONE))
			LearningAgentsManager->RemoveAgent(AgentId);
	}
	else if (PendingAgents.Contains(Pawn))
		PendingAgents.Remove(Pawn);
}

void ULearningAgentSubsystem::UnregisterImitationLearningRecordingManager(
	ALearningAgentsImitationCombatRecordingManager* LearningAgentsImitationCombatRecordingManager)
{
	if (LearningAgentsImitationCombatRecordingManager == ILRecordingManager)
	{
		ILRecordingManager.Reset();
	}
}

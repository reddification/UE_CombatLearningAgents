// 


#include "Subsystems/MLSubsystem.h"

#include "LearningAgentsManager.h"

UMLSubsystem* UMLSubsystem::Get(const UObject* WorldContextObject)
{
	return IsValid(WorldContextObject) ? WorldContextObject->GetWorld()->GetSubsystem<UMLSubsystem>() : nullptr;
}

void UMLSubsystem::RegisterManager(AMLManagerBase* Manager, const FGameplayTag& BehaviorTag)
{
	if (MLManagers.Contains(BehaviorTag))
	{
		ensure(false);
		return;
	}
	
	MLManagers.Add(BehaviorTag, Manager);
	ManagersAgents.Add(BehaviorTag);
	MLManagerChangedEvent.Broadcast(BehaviorTag, Manager);
}

void UMLSubsystem::UnregisterManager(AMLManagerBase* Manager, const FGameplayTag& BehaviorTag)
{
	if (MLManagers.Contains(BehaviorTag) && ensure(MLManagers[BehaviorTag] == Manager))
	{
		MLManagers[BehaviorTag]->GetLearningAgentsManager()->RemoveAllAgents();
		ManagersAgents.Remove(BehaviorTag);
		MLManagers.Remove(BehaviorTag);
		MLManagerChangedEvent.Broadcast(BehaviorTag, nullptr);
	}
}

AMLManagerBase* UMLSubsystem::GetManager(const FGameplayTag& BehaviorTag) const
{
	return MLManagers.Contains(BehaviorTag) ? MLManagers[BehaviorTag].Get() : nullptr;
}

bool UMLSubsystem::RegisterMLAgent(APawn* Pawn, const FGameplayTag& BehaviorTag)
{
	if (!MLManagers.Contains(BehaviorTag))
		return false;
	
	auto LAM = MLManagers[BehaviorTag]->GetLearningAgentsManager();
	int RegisteredIndex = LAM->AddAgent(Pawn);
	if (ensure(RegisteredIndex != INDEX_NONE))
	{
		ManagersAgents[BehaviorTag].Add(Pawn, RegisteredIndex);
		Pawn->AddTickPrerequisiteComponent(LAM);
		if (ManagersAgents[BehaviorTag].Num() == 1)
			MLManagers[BehaviorTag]->SetManagerActive(true);
			
		return true;
	}
	
	return false;
}

bool UMLSubsystem::UnregisterMLAgent(APawn* Pawn, const FGameplayTag& BehaviorTag)
{
	if (!MLManagers.Contains(BehaviorTag) || !ManagersAgents[BehaviorTag].Contains(Pawn))
		return false;

	auto& Manager = MLManagers[BehaviorTag];
	auto& Agents = ManagersAgents[BehaviorTag];
	auto LAM = Manager->GetLearningAgentsManager();
	LAM->RemoveAgent(Agents[Pawn]);
	Pawn->RemoveTickPrerequisiteComponent(LAM);
	Agents.Remove(Pawn);
	if (Agents.IsEmpty())
		Manager->SetManagerActive(false);
	
	return true;
}

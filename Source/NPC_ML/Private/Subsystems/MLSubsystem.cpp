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
	RegisteredAgents.Add(BehaviorTag);
}

void UMLSubsystem::UnregisterManager(AMLManagerBase* Manager, const FGameplayTag& BehaviorTag)
{
	if (MLManagers.Contains(BehaviorTag) && ensure(MLManagers[BehaviorTag] == Manager))
	{
		MLManagers[BehaviorTag]->GetLearningAgentsManager()->RemoveAllAgents();
		RegisteredAgents.Remove(BehaviorTag);
		MLManagers.Remove(BehaviorTag);
	}
}

bool UMLSubsystem::RegisterMLAgent(APawn* Pawn, const FGameplayTag& BehaviorTag)
{
	if (!MLManagers.Contains(BehaviorTag))
		return false;
	
	auto LAM = MLManagers[BehaviorTag]->GetLearningAgentsManager();
	int RegisteredIndex = LAM->AddAgent(Pawn);
	if (ensure(RegisteredIndex != INDEX_NONE))
	{
		RegisteredAgents[BehaviorTag].Add(Pawn, RegisteredIndex);
		Pawn->AddTickPrerequisiteComponent(LAM);
		if (RegisteredAgents[BehaviorTag].Num() == 1)
			MLManagers[BehaviorTag]->SetManagerActive(true);
			
		return true;
	}
	
	return false;
}

bool UMLSubsystem::UnregisterMLAgent(APawn* Pawn, const FGameplayTag& BehaviorTag)
{
	if (!MLManagers.Contains(BehaviorTag) || !RegisteredAgents[BehaviorTag].Contains(Pawn))
		return false;

	auto& Manager = MLManagers[BehaviorTag];
	auto& Agents = RegisteredAgents[BehaviorTag];
	auto LAM = Manager->GetLearningAgentsManager();
	LAM->RemoveAgent(Agents[Pawn]);
	Pawn->RemoveTickPrerequisiteComponent(LAM);
	Agents.Remove(Pawn);
	if (Agents.IsEmpty())
		Manager->SetManagerActive(false);
	
	return true;
}

void UMLSubsystem::UnregisterILRecordingManager(AImitationLearningRecordingManager* InILRecordingManager)
{
	if (InILRecordingManager == ILRecordingManager)
		ILRecordingManager.Reset();
}

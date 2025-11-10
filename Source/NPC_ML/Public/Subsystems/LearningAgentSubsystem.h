// 

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "LearningAgentSubsystem.generated.h"

class ULearningAgentsManager;
class UCombatLearningAgentsManagerComponent;
/**
 * 
 */
UCLASS()
class NPC_ML_API ULearningAgentSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RegisterLearningAgentsManager(ULearningAgentsManager* LearningAgentsManager);
	void UnregisterLearningAgentsManager(ULearningAgentsManager* LearningAgentsManager);

	void RegisterLearningAgent(APawn* Pawn);
	void UnregisterLearningAgent(APawn* Pawn);
	
private:
	TWeakObjectPtr<ULearningAgentsManager> LearningAgentsManager;
	TArray<TWeakObjectPtr<APawn>> PendingAgents;
	TMap<TWeakObjectPtr<APawn>, int> RegisteredAgents;
};

// 

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "LearningAgentSubsystem.generated.h"

class ALearningAgentsImitationCombatRecordingManager;
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

	void RegisterImitationLearningRecordingManager(ALearningAgentsImitationCombatRecordingManager* InILRecordingManager) { ILRecordingManager = InILRecordingManager; };
	void UnregisterImitationLearningRecordingManager(ALearningAgentsImitationCombatRecordingManager* LearningAgentsImitationCombatRecordingManager);;
	ALearningAgentsImitationCombatRecordingManager* GetLearningAgentsImitationCombatRecordingManager() const { return ILRecordingManager.Get(); };
	
private:
	TWeakObjectPtr<ULearningAgentsManager> LearningAgentsManager;
	TWeakObjectPtr<ALearningAgentsImitationCombatRecordingManager> ILRecordingManager;
	TArray<TWeakObjectPtr<APawn>> PendingAgents;
	TMap<TWeakObjectPtr<APawn>, int> RegisteredAgents;
};

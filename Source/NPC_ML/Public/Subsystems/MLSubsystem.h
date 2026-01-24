// 

#pragma once

#include "CoreMinimal.h"
#include "Actors/IL/ImitationLearningRecordingManager.h"
#include "Subsystems/WorldSubsystem.h"
#include "MLSubsystem.generated.h"

class ULAInferenceManagerComponent;
class ULearningAgentsManager;
class ULAReinforcementLearningManagerComponent;
/**
 * 
 */
UCLASS()
class NPC_ML_API UMLSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UMLSubsystem* Get(const UObject* WorldContextObject);
	
	void RegisterManager(AMLManagerBase* Manager, const FGameplayTag& BehaviorTag);
	void UnregisterManager(AMLManagerBase* Manager, const FGameplayTag& BehaviorTag);
	
	bool RegisterMLAgent(APawn* Pawn, const FGameplayTag& BehaviorTag);
	bool UnregisterMLAgent(APawn* Pawn, const FGameplayTag& BehaviorTag);

	void RegisterILRecordingManager(AImitationLearningRecordingManager* InILRecordingManager) { ILRecordingManager = InILRecordingManager; }
	void UnregisterILRecordingManager(AImitationLearningRecordingManager* InILRecordingManager);
	AImitationLearningRecordingManager* GetILRecordingManager() const { return ILRecordingManager.Get(); }

private:
	TWeakObjectPtr<AImitationLearningRecordingManager> ILRecordingManager;
	TMap<FGameplayTag, TWeakObjectPtr<AMLManagerBase>> MLManagers;
	TMap<FGameplayTag, TMap<TWeakObjectPtr<APawn>, int>> RegisteredAgents;
};

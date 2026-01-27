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

private:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FMLManagerChangedEvent, FGameplayTag ManagerTag, AMLManagerBase* ChangedManager);
	
public:
	static UMLSubsystem* Get(const UObject* WorldContextObject);
	
	void RegisterManager(AMLManagerBase* Manager, const FGameplayTag& BehaviorTag);
	void UnregisterManager(AMLManagerBase* Manager, const FGameplayTag& BehaviorTag);
	AMLManagerBase* GetManager(const FGameplayTag& BehaviorTag) const;
	
	bool RegisterMLAgent(APawn* Pawn, const FGameplayTag& BehaviorTag);
	bool UnregisterMLAgent(APawn* Pawn, const FGameplayTag& BehaviorTag);

	mutable FMLManagerChangedEvent MLManagerChangedEvent;
	
private:
	TMap<FGameplayTag, TWeakObjectPtr<AMLManagerBase>> MLManagers;
	TMap<FGameplayTag, TMap<TWeakObjectPtr<APawn>, int>> ManagersAgents;
};

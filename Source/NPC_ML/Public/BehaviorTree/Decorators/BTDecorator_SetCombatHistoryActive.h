// 

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_SetCombatHistoryActive.generated.h"

/**
 * Assumption your NPCs are controlled by a behavior tree on a global event, 
 */
UCLASS()
class NPC_ML_API UBTDecorator_SetCombatHistoryActive : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_SetCombatHistoryActive();
	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;
	virtual void OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult) override;
};

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RunMachineLearningBehavior.generated.h"

/**
 * 
 */
UCLASS()
class NPC_ML_API UBTTask_RunMachineLearningBehavior : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_RunMachineLearningBehavior();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
	
protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag BehaviorTag;
	
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual void OnMessage(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, FName Message, int32 RequestID, bool bSuccess) override;
};

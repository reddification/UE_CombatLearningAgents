#include "BehaviorTree/Tasks/BTTask_RunMachineLearningBehavior.h"

#include "AIController.h"
#include "Subsystems/MLSubsystem.h"

UBTTask_RunMachineLearningBehavior::UBTTask_RunMachineLearningBehavior()
{
	NodeName = "Run Machine Learning Behavior";
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_RunMachineLearningBehavior::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!BehaviorTag.IsValid())
		return EBTNodeResult::Failed;
	
	auto MLS = UMLSubsystem::Get(OwnerComp.GetWorld());
	if (!MLS)
		return EBTNodeResult::Failed;

	if (MLS->RegisterMLAgent(OwnerComp.GetAIOwner()->GetPawn(), BehaviorTag))
	{
		WaitForMessage(OwnerComp, FName("StopInference"));
		return EBTNodeResult::InProgress;
	}
	
	return EBTNodeResult::Failed;
}

void UBTTask_RunMachineLearningBehavior::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                        EBTNodeResult::Type TaskResult)
{
	if (TaskResult == EBTNodeResult::Failed)
		return;
	
	auto WorldLocal = OwnerComp.GetWorld();
	auto MLS = UMLSubsystem::Get(WorldLocal);
	if (!MLS)
		return;
	
	auto AIController = OwnerComp.GetAIOwner();
	if (!IsValid(AIController)) return;
	
	auto Pawn = AIController->GetPawn();
	if (!IsValid(Pawn)) return;
	
	MLS->UnregisterMLAgent(Pawn, BehaviorTag);
	
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_RunMachineLearningBehavior::OnMessage(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, FName Message,
	int32 RequestID, bool bSuccess)
{
	Super::OnMessage(OwnerComp, NodeMemory, Message, RequestID, bSuccess);
}

FString UBTTask_RunMachineLearningBehavior::GetStaticDescription() const
{
	return FString::Printf(TEXT("Run inference\n%s"), *BehaviorTag.ToString());
}

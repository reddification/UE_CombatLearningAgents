// 


#include "BehaviorTree/Decorators/BTDecorator_SetCombatHistoryActive.h"

#include "AIController.h"
#include "Components/LAObservationHistoryComponent.h"

UBTDecorator_SetCombatHistoryActive::UBTDecorator_SetCombatHistoryActive()
{
	NodeName = "Set combat history active";
	bNotifyActivation = true;
	bNotifyDeactivation = true;
}

void UBTDecorator_SetCombatHistoryActive::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
	Super::OnNodeActivation(SearchData);
	auto CombatHistoryComponent = SearchData.OwnerComp.GetAIOwner()->GetPawn()->FindComponentByClass<ULAObservationHistoryComponent>();
	CombatHistoryComponent->SetHistoryActive(true);
}

void UBTDecorator_SetCombatHistoryActive::OnNodeDeactivation(FBehaviorTreeSearchData& SearchData,
	EBTNodeResult::Type NodeResult)
{
	auto AIController = SearchData.OwnerComp.GetAIOwner();
	if (IsValid(AIController))
	{
		auto Pawn = AIController->GetPawn();
		if (IsValid(Pawn))
		{
			auto CombatHistoryComponent = Pawn->FindComponentByClass<ULAObservationHistoryComponent>();
			CombatHistoryComponent->SetHistoryActive(false);			
		}
	}

	Super::OnNodeDeactivation(SearchData, NodeResult);
}

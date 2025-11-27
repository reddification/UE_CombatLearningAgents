// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LearningAgentCombatActionsComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NPC_ML_API ULearningAgentCombatActionsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULearningAgentCombatActionsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	// called on each inference before sampling action
	virtual void Reset() {  }
	
	virtual void Attack(int32 AttackIndex) {}
	virtual void Parry(const FVector& ParryDirection) {}
	virtual void Dodge(const FVector& DodgeDirection) {}
};

// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LACombatActionsComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NPC_ML_API ULACombatActionsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULACombatActionsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	// called on each inference before sampling action
	virtual void Reset() {  }
	
	virtual void Attack(int32 AttackIndex) {}
	// Convention: parry angle is an angle 0->360 on a unit circle starting with 0 (and 360) at XY: (1, 0)
	virtual void Block(float ParryAngle) {}
	virtual void Dodge(const FVector& DodgeDirection) {}
};

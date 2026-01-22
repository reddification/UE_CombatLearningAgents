// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "LearningAgentLocomotionActionsComponent.generated.h"

/*
 * Put child of this component on your NPC and override virtual functions
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NPC_ML_API ULearningAgentLocomotionActionsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void Reset() { CurrentMoveInput = FVector::ZeroVector; CurrentRotationInput = FRotator::ZeroRotator; }
	virtual void Jump() {}
	virtual void Mantle() {}
	virtual void SetSpeed(float NewSpeed) {}
	virtual void AddMoveInput(const FVector& MoveDirection) { CurrentMoveInput = MoveDirection; }
	virtual void AddRotationInput(const FRotator& Rotator) { CurrentRotationInput = Rotator; }
	virtual void Gesture(const FGameplayTag& GestureTag) {}
	virtual void SayPhrase(const FGameplayTag& PhraseTag) {}
	virtual void UseItem(const FGameplayTag& ItemId) {}
	virtual void SetWeaponReady(bool bReady) {}

protected:
	virtual void BeginPlay() override;
	virtual void UpdateLocomotionInputs();
	
	FVector CurrentMoveInput = FVector::ZeroVector;
	FRotator CurrentRotationInput = FRotator::ZeroRotator;
	
private:
	TWeakObjectPtr<ACharacter> CharacterOwner;
};

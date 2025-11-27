// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "LearningAgentLocomotionActionsComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NPC_ML_API ULearningAgentLocomotionActionsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULearningAgentLocomotionActionsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void Reset() { CurrentMoveInput = FVector::ZeroVector; }
	virtual void Jump(const FVector& Destination) {}
	virtual void Climb(const FVector& Direction) {}
	virtual void SetSpeed(float NewSpeed) {}
	virtual void SetMoveDirection(const FVector& MoveDirection) { CurrentMoveInput = MoveDirection; }
	virtual void SetRotator(const FRotator& Rotator) {}
	virtual void Gesture(const FGameplayTag& GestureTag) {}
	virtual void SayPhrase(const FGameplayTag& PhraseTag) {}
	virtual void UseItem(int32 ItemId) {};

private:
	FVector CurrentMoveInput = FVector::ZeroVector;
};

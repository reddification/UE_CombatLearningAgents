#include "Components/LearningAgentLocomotionActionsComponent.h"
#include "GameFramework/Character.h"

void ULearningAgentLocomotionActionsComponent::BeginPlay()
{
	Super::BeginPlay();
	CharacterOwner = Cast<ACharacter>(GetOwner());
}

void ULearningAgentLocomotionActionsComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateLocomotionInputs();
}

void ULearningAgentLocomotionActionsComponent::UpdateLocomotionInputs()
{
	if (CurrentMoveInput != FVector::ZeroVector)
		CharacterOwner->AddMovementInput(CurrentMoveInput);
	
	if (CurrentRotationInput != FRotator::ZeroRotator)
	{
		CharacterOwner->AddControllerPitchInput(CurrentRotationInput.Pitch);
		CharacterOwner->AddControllerYawInput(CurrentRotationInput.Yaw);
	}
}

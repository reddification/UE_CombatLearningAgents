#include "Components/LALocomotionActionsComponent.h"
#include "GameFramework/Character.h"

void ULALocomotionActionsComponent::BeginPlay()
{
	Super::BeginPlay();
	CharacterOwner = Cast<ACharacter>(GetOwner());
}

void ULALocomotionActionsComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateLocomotionInputs();
}

void ULALocomotionActionsComponent::UpdateLocomotionInputs()
{
	if (CurrentMoveInput != FVector::ZeroVector)
		CharacterOwner->AddMovementInput(CurrentMoveInput);
	
	if (CurrentRotationInput != FRotator::ZeroRotator)
	{
		CharacterOwner->AddControllerPitchInput(CurrentRotationInput.Pitch);
		CharacterOwner->AddControllerYawInput(CurrentRotationInput.Yaw);
	}
}

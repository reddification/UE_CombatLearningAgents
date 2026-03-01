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
	UpdateLocomotionInputs(DeltaTime);
}

void ULALocomotionActionsComponent::UpdateLocomotionInputs(float DeltaTime)
{
	// 26 Feb 2026 (aki)
	// should I multiply inputs by delta time? idk. atm I assume that NO,
	// because IIUC agents produce actions without understanding inference tick rate 
	// so to them they infere actions from observations all the time 
	// and not in a manner like "for whatever time there is to the next inference apply in total this input: X" 
	if (CurrentMoveInput != FVector::ZeroVector)
		CharacterOwner->AddMovementInput(CurrentMoveInput);
	
	if (CurrentRotationInput != FRotator::ZeroRotator)
	{
		CharacterOwner->AddControllerPitchInput(CurrentRotationInput.Pitch);
		CharacterOwner->AddControllerYawInput(CurrentRotationInput.Yaw);
	}
}

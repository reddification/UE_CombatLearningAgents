#pragma once

#include "LearningAgentsDataTypes.generated.h"

// will be used as bitflag so can't be uint8
UENUM()
enum ELACharacterStates
{
	None = 0,
	Attacking = 1,
	Dodging = Attacking << 1,
	Parrying = Dodging << 1,
	Blocking = Parrying << 1,
	Covering = Blocking << 1,
	ReactingToDamage = Covering << 1,
	InStagger = ReactingToDamage << 1,
	Sprinting = InStagger << 1,
	Sneaking = Sprinting << 1,
	WeaponSheathed = Sneaking << 1,
	WeaponUnsheathed = WeaponSheathed << 1,
	Falling = WeaponUnsheathed << 1,
	Swimming = Falling << 1,
	WalkingOnGround = Swimming << 1,
	Dying = WalkingOnGround << 1,
	Gesturing = Dying << 1,
	HasActiveRootMotionAnimation = Gesturing << 1,
	Speaking = HasActiveRootMotionAnimation << 1,
	UsingConsumableItem = Speaking << 1,
	InteractingWithObject = UsingConsumableItem << 1,
};


UENUM()
enum class ECombatEvent : uint8
{
	None = 0,
	Approach,
	Combat,
	Disengage,
	Retreat,
	ProcessVictory,
	Stop
};

UENUM()
enum class ELAActiveWeaponType : uint8
{
	Unarmed = 0,
	Melee,
	Ranged,
};

USTRUCT(BlueprintType)
struct FLidarRaindropParams
{
	GENERATED_BODY()

	FLidarRaindropParams() : Radius(1000.f), Density(15.f) { }
	FLidarRaindropParams(float InRadius, float InDensity) : Radius(InRadius), Density(InDensity) {  }
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int Radius = 1000;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int Density = 15;
};

struct FLidarObservationCache
{
	float AverageCeilingHeight = 500.f;
	// essentially, matrices, but since I'm going to shove them into conv2d observations as static arrays,
	// it's more conventient to store them as 1d array from the get-go
	TArray<float> DownwardRaindrops; 
	TArray<float> ForwardRaindrops;
	TArray<float> BackwardRaindrops;
};
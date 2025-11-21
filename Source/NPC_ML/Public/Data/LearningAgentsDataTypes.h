#pragma once

#include "LearningAgentsDataTypes.generated.h"

// will be used as bitflag so can't be uint8
UENUM()
enum class ELACombatState
{
	None,
	Attacking,
	Dodging,
	InStagger,
	Blocking,
	Sprinting,
	Sneaking,
	WeaponSheathed,
	WeaponUnsheathed
};

UENUM()
enum class ECombatEvents : uint8
{
	None = 0,
	Staggered,
	ParriedAttack,
	WasParried,
	Died,
	StartedSprinting,
	StoppedSprinting,
	SheathedWeapon,
	UnsheatheWeapon,
	Gesture
};

UENUM()
enum class ELAActiveWeaponType : uint8
{
	Unarmed = 0,
	Melee,
	Ranged,
};

UENUM()
enum class ELAGesture : uint8
{
	None = 0,
	Taunt,
	ShowOff,
	Provoke,
	PointAt,
	ShowRespect,
	Frightened,
	BegMercy,
	CallForHelp,
	DontDoIt,
	PutYourWeaponAway,
	GoAway
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
	
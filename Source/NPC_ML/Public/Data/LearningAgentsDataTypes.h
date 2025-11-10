#pragma once

#include "LearningAgentsDataTypes.generated.h"

// will be used as bitflag so can't be uint8
UENUM()
enum class ELACombatStates
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

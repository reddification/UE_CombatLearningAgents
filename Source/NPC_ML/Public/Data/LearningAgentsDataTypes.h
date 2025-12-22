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
	WeaponNotReady = Sneaking << 1,
	WeaponReady = WeaponNotReady << 1,
	ChangingWeaponState = WeaponReady << 1,
	Falling = ChangingWeaponState << 1,
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

UENUM()
enum class ELAWeaponStateChange : uint8
{
	Unready = 0,
	Ready = 1
};

UENUM()
enum class ELALocomotionAction : uint8
{
	Jump = 0,
	Mantle = 1
};

UENUM()
enum class ELAAttackType : uint8
{
	Default = 0
};
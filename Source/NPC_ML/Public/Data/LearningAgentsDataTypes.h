#pragma once

#include "LearningAgentsDataTypes.generated.h"

// will be used as bitflag so can't be uint8
UENUM()
enum class ELACharacterStates
{
	None = 0,

	Attacking              		 = 1u << None,
	RecoveringAfterAttack  		 = Attacking << 1,
	Dodging                		 = RecoveringAfterAttack << 1,
	Blocking               		 = Dodging << 1,
	Covering               		 = Blocking << 1,
	InStagger              		 = Covering << 1,
	Sprinting              		 = InStagger << 1,
	Sneaking               		 = Sprinting << 1,
	WeaponNotReady         		 = Sneaking << 1,
	WeaponReady            		 = WeaponNotReady << 1,
	ChangingWeaponState    		 = WeaponReady << 1,
	InAir                		 = ChangingWeaponState << 1,
	Swimming               		 = InAir << 1,
	WalkingOnGround        		 = Swimming << 1,
	Dying                  		 = WalkingOnGround << 1,
	Gesturing              		 = Dying << 1,
	HasActiveRootMotionAnimation = Gesturing << 1,
	Speaking                	 = HasActiveRootMotionAnimation << 1,
	UsingConsumableItem     	 = Speaking << 1,
	InteractingWithObject   	 = UsingConsumableItem << 1,
};

ENUM_CLASS_FLAGS(ELACharacterStates);

UENUM()
enum class ELACombatEvent : uint8
{
	None = 0,
	AttackMissed,
	DealtHit,
	Dodged,
	Parried,
	Blocked,
	Killed,
	GotParried,
	GotDodged,
	GotBlocked,
	GotHit,
};

UENUM()
enum class ELACombatBehavior : uint8
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

struct FCombatEventData
{
	FCombatEventData(float WorldTime, ELACombatEvent CombatEvent)
		: AtWorldTime(WorldTime), CombatEvent(CombatEvent)
	{
	}

	float AtWorldTime = 0.f;
	ELACombatEvent CombatEvent = ELACombatEvent::None;
};

struct FTranslationHistory
{
	FTranslationHistory(AActor* Actor)
		: Velocity(Actor->GetVelocity()), Transform(Actor->GetTransform())
	{
		Location = Transform.GetLocation();
		Rotation = Transform.GetRotation().Rotator();
	}
	
	FTranslationHistory(const FVector& InVelocity, const FTransform& InTransform)
		: Velocity(InVelocity), Transform(InTransform)
	{
		Location = InTransform.GetLocation();
		Rotation = InTransform.GetRotation().Rotator();
	}
	
	FVector Velocity = FVector::ZeroVector;
	
	// this is transform of the history owner
	FVector Location;
	FRotator Rotation;
	FTransform Transform = FTransform::Identity;
	
	// this is the transform of the agent for which this translation history is observed
	// so if A1 (agent 1) is observing translation history of A2, then 
	// Transform <- A2
	// RelativeTransform <- A1
	FTransform RelativeTransform = FTransform::Identity;
};

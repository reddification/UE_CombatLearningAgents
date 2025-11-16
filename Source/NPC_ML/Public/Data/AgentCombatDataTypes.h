#pragma once

#include "LearningAgentsDataTypes.h"

enum class ELAGesture : uint8;
enum class ELAActiveWeaponType : uint8;
enum class ELACombatState;

namespace CombatLearning
{
	struct FWeaponData
	{
		float AttackRange = 0.f;
		float NormalizedWeaponMastery = 1.f;
		ELAActiveWeaponType WeaponType;
		float PowerRate = 10.f;
	};

	struct FCharacterBaseData
	{
		float NormalizedHealth = 1.f;
		FVector WorldVelocity = FVector::ZeroVector;
		ELACombatState CombatStates;
		FWeaponData WeaponData;
		int Level = 0;
		float ArmorRate;
		ELAGesture ActiveGesture = ELAGesture::None;
	};

	struct FOtherCharacterData : public FCharacterBaseData
	{
		AActor* Actor;
		bool bAlive = true;
		bool bAgentCanSeeCharacter = true;
		bool bCharacterCanSeeAgent = true;
	};
	
	struct FSelfData : public FCharacterBaseData
	{
		float NormalizedStamina = 1.f;
		float SurvivalDesire = 0.f;
	};

	struct FEnemyData : public FOtherCharacterData
	{
		float KillDesire = 1.f;
	};

	struct FAllyData : public FOtherCharacterData
	{
		AActor* AllyActor;
	};

	struct FCombatStateData
	{
		float CombatTotalDuration = 0.f;
	};
}
#pragma once

#include "GameplayTagContainer.h"
#include "LearningAgentsDataTypes.h"

namespace CombatLearning
{
	struct FWeaponData
	{
		float AttackRange = 0.f;
		float NormalizedWeaponMastery = 1.f;
		ELAActiveWeaponType WeaponType;
		float PowerRate = 10.f;
	};

	struct FRoleplayIdentity
	{
		uint8 Personality = 0;
		uint8 Temper = 0;
		uint8 Objective = 0;
	};
	
	struct FCharacterDataBase
	{
		float NormalizedHealth = 1.f;
		TWeakObjectPtr<const AActor> Actor = nullptr;
		FVector WorldVelocity = FVector::ZeroVector;
		ELACharacterStates CombatStates;
		FWeaponData WeaponData;
		float ArmorRate = 0.f;
		float AccumulatedNormalizedDamage = 0.f;
		FGameplayTag ActiveGesture;
		FGameplayTag ActivePhrase;
		FRoleplayIdentity Identity;
		uint8 ActiveAttack = 0;
	};

	struct FPerceivedCharacterData : public FCharacterDataBase
	{
		bool bAlive = true;
		bool bAgentCanSeeCharacter = true;
		bool bCharacterCanSeeAgent = true;
		uint8 RoleplayAttitude = 0;
	};
	
	struct FSelfData : public FCharacterDataBase
	{
		float NormalizedStamina = 1.f;
		float NormalizedPoise = 1.f;
	};

	struct FEnemyData : public FPerceivedCharacterData
	{
		// empty for now as of 8 Mar 2026
	};

	struct FAllyData : public FPerceivedCharacterData
	{
		// empty for now as of 8 Mar 2026
	};
	
	struct FCombatStateData
	{
		float CombatTotalDuration = 0.f;
		int EnemiesCount = 1;
		int AlliesCount = 0;
	};
}
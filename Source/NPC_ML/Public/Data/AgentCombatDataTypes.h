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
		FVector WorldVelocity = FVector::ZeroVector;
		ELACharacterStates CombatStates;
		FWeaponData WeaponData;
		int Level = 0;
		float ArmorRate = 0.f;
		float AccumulatedNormalizedDamage = 0.f;
		FGameplayTag ActiveGesture;
		FGameplayTag ActivePhrase;
		FRoleplayIdentity Identity;
	};

	struct FPerceivedCharacterData : public FCharacterDataBase
	{
		TWeakObjectPtr<const AActor> Actor = nullptr;
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
		// this is not personal attention to enemy at this moment of time but "how important in general for the agent to kill the target"
		// so for example a wanderer would have 0.25 kill desire against rabid dog,
		//		like if the dog is likely to kill the agent then fuck it lets run, or don't really push it and approach carefully
		// and if you just killed agent's "wife" then it will be furious with kill desire == 1 
	};

	struct FAllyData : public FPerceivedCharacterData
	{
	};
	
	struct FCombatStateData
	{
		float CombatTotalDuration = 0.f;
		int EnemiesCount = 1;
		int AlliesCount = 0;
	};
}
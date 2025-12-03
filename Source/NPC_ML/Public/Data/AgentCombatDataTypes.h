#pragma once

#include "GameplayTagContainer.h"
#include "LearningAgentsDataTypes.h"

enum class ELAActiveWeaponType : uint8;
enum ELACharacterStates;

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
		ELACharacterStates CombatStates;
		FWeaponData WeaponData;
		int Level = 0;
		float ArmorRate;
		FGameplayTag ActiveGesture;
		FGameplayTag ActivePhrase;
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
		int EnemiesCount = 1;
		int AlliesCount = 0;
	};
}
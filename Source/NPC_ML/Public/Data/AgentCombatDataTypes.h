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

	struct FCharacterStateBase
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

	struct FOtherCharacterState : public FCharacterStateBase
	{
		TWeakObjectPtr<AActor> Actor = nullptr;
		bool bAlive = true;
		bool bAgentCanSeeCharacter = true;
		bool bCharacterCanSeeAgent = true;
	};
	
	struct FSelfData : public FCharacterStateBase
	{
		float NormalizedStamina = 1.f;
		float SurvivalDesire = 0.f;
	};

	struct FEnemyState : public FOtherCharacterState
	{
		// this is not personal attention to enemy at this moment of time but "how important in general for the agent to kill the target"
		// so for example a wanderer would have 0.25 kill desire against rabid dog,
		//		like if the dog is likely to kill the agent then fuck it lets run, or don't really push it and approach carefully
		// and if you just killed agent's "wife" then it will be furious with kill desire == 1 
		float KillDesire = 1.f;
	};

	struct FAllyState : public FOtherCharacterState
	{
	};
	
	struct FCombatStateData
	{
		float CombatTotalDuration = 0.f;
		int EnemiesCount = 1;
		int AlliesCount = 0;
	};
}
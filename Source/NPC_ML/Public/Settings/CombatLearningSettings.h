// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "CombatLearningSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, defaultconfig, DisplayName="Combat learning")
class NPC_ML_API UCombatLearningSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config)
	FGameplayTagContainer RelevantObservations;	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Normalization", meta=(UIMin = 1.f, ClampMin = 1.f))
	float LIDARDistanceScale = 2000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Normalization", meta=(UIMin = 1.f, ClampMin = 1.f))
	float EnemyDistanceScale = 1000.f;

	// max "how hard this guy hits" value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Normalization", meta=(ClampMin = 0.0001f, UIMin = 0.0001f))
	float MaxPowerRate = 250.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Normalization", meta=(ClampMin = 0.0001f, UIMin = 0.0001f))
	float MaxArmorRate = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Normalization", meta=(ClampMin = 1.f, UIMin = 1.f))
	float MaxSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Normalization", meta=(ClampMin = 1.f, UIMin = 1.f))
	float MaxLevel = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Normalization", meta=(ClampMin = 0.0001f, UIMin = 0.0001f))
	float MaxMeleeAttackRange = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Normalization", meta=(ClampMin = 0.0001f, UIMin = 0.0001f))
	float MaxRangedAttackRange = 3000.f;
	
	// when observing walkable paths, make 360 / this value observations around NPC
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, meta=(ClampMin = 1, UIMin = 1))
	int32 WalkablePathsDirectionsAngleDelta = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, meta=(ClampMin = 1, UIMin = 1))
	int MaxEnemiesObservedAtOnce = 4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, meta=(ClampMin = 1, UIMin = 1))
	int MaxAlliesObservedAtOnce = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, meta=(ClampMin = 1.f, UIMin = 1.f))
	float MaxExpectedCombatDuration = 120.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config)
	bool bVisLogEnabled = true;
};

// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "LearningAgentsObservations.h"
#include "Data/LearningAgentsDataTypes.h"
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
	FLidarRaindropParams DownwardRaindropsParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config)
	FLidarRaindropParams ForwardRaindropsParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config)
	FLidarRaindropParams BackwardRaindropsParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config)
	FConv2dObservationParams LidarRaindropDownwardConv2dParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config)
	FConv2dObservationParams LidarRaindropForwardConv2dParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config)
	FConv2dObservationParams LidarRaindropBackwardConv2dParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config)
	bool bVisLogEnabled = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config)
	float MaxCeilingHeight = 1500.f;
	
	FORCEINLINE int GetDownwardRaindropResolution() const { return DownwardRaindropsParams.Radius * 2 / DownwardRaindropsParams.Density; }
	FORCEINLINE int GetForwardRaindropResolution() const { return ForwardRaindropsParams.Radius * 2 / ForwardRaindropsParams.Density; }
	FORCEINLINE int GetBackwardRaindropResolution() const { return BackwardRaindropsParams.Radius *2 / BackwardRaindropsParams.Density; }

};

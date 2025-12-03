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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations", meta=(UIMin = 1.f, ClampMin = 1.f))
	float LIDARDistanceScale = 2000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations", meta=(UIMin = 1.f, ClampMin = 1.f))
	float EnemyDistanceScale = 1000.f;

	// max "how hard this guy hits" value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations", meta=(ClampMin = 0.0001f, UIMin = 0.0001f))
	float MaxPowerRate = 250.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations", meta=(ClampMin = 0.0001f, UIMin = 0.0001f))
	float MaxArmorRate = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations", meta=(ClampMin = 1.f, UIMin = 1.f))
	float MaxSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations", meta=(ClampMin = 1.f, UIMin = 1.f))
	float MaxJumpDistance = 250.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations")
	float RotationScale = 90.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations", meta=(ClampMin = 1.f, UIMin = 1.f))
	float MaxLevel = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations", meta=(ClampMin = 0.0001f, UIMin = 0.0001f))
	float MaxMeleeAttackRange = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations", meta=(ClampMin = 0.0001f, UIMin = 0.0001f))
	float MaxRangedAttackRange = 3000.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, meta=(ClampMin = 1, UIMin = 1), Category="Observations")
	int MaxEnemiesObservedAtOnce = 4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, meta=(ClampMin = 1, UIMin = 1), Category="Observations")
	int MaxAlliesObservedAtOnce = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, meta=(ClampMin = 1.f, UIMin = 1.f), Category="Observations")
	float MaxExpectedCombatDuration = 120.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|LIDAR")
	FLidarRaindropParams DownwardRaindropsParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|LIDAR")
	FLidarRaindropParams ForwardRaindropsParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|LIDAR")
	FLidarRaindropParams BackwardRaindropsParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|LIDAR")
	FConv2dObservationParams LidarRaindropDownwardConv2dParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|LIDAR")
	FConv2dObservationParams LidarRaindropForwardConv2dParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|LIDAR")
	FConv2dObservationParams LidarRaindropBackwardConv2dParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|LIDAR")
	float MaxCeilingHeight = 1500.f;
	
	// values - probabilities
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Actions")
	TMap<FGameplayTag, float> Gestures;
	
	// values - probabilities
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Actions")
	TMap<FGameplayTag, float> Phrases;

	// values - probabilities
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Actions")
	TMap<FGameplayTag, float> ConsumablesProbabilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Actions")
	TArray<float> AttackActionsProbabilities;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config)
	bool bVisLogEnabled = true;
	

	FORCEINLINE int GetDownwardRaindropResolution() const { return DownwardRaindropsParams.Radius * 2 / DownwardRaindropsParams.Density; }
	FORCEINLINE int GetForwardRaindropResolution() const { return ForwardRaindropsParams.Radius * 2 / ForwardRaindropsParams.Density; }
	FORCEINLINE int GetBackwardRaindropResolution() const { return BackwardRaindropsParams.Radius *2 / BackwardRaindropsParams.Density; }

};

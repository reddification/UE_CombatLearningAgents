#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "LearningAgentsObservations.h"
#include "Data/RaindropDataTypes.h"
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations", meta=(UIMin = 1.f, ClampMin = 1.f))
	float EnemyDistanceScale = 1000.f;

	// max "how hard this guy hits" value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations", meta=(ClampMin = 0.0001f, UIMin = 0.0001f))
	float MaxPowerRate = 250.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations", meta=(ClampMin = 0.0001f, UIMin = 0.0001f))
	float MaxArmorRate = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations", meta=(ClampMin = 1.f, UIMin = 1.f))
	float MaxSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations")
	float RotationScale = 90.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations", meta=(ClampMin = 1.f, UIMin = 1.f))
	float MaxLevel = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations", meta=(ClampMin = 0.0001f, UIMin = 0.0001f))
	float MaxMeleeAttackRange = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations", meta=(ClampMin = 0.0001f, UIMin = 0.0001f))
	float MaxRangedAttackRange = 3000.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, meta=(ClampMin = 1, UIMin = 1), Category="Observations")
	int MaxEnemiesObservedAtOnce = 4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, meta=(ClampMin = 1, UIMin = 1), Category="Observations")
	int MaxAlliesObservedAtOnce = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, meta=(ClampMin = 1.f, UIMin = 1.f), Category="Observations")
	float MaxExpectedCombatDuration = 120.f;

	// if true - then you must also specify combat history in interactor relevant observations in UMLModelVersion data asset
	// 1 Mar 2026 (aki): this is bullshit. TODO refactor so that combat history observation relevancy is managed in one place 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|History")
	bool bKeepCombatObservationHistory = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|History", meta=(EditCondition="bKeepCombatObservationHistory"))
	float TranslationHistoryUpdateInterval = 0.3f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|History", meta=(EditCondition="bKeepCombatObservationHistory"))
	int TranslationHistorySize = 20;
	
	// used for normalization. how much time on average passes between combat encounters
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|History", meta=(EditCondition="bKeepCombatObservationHistory"))
	float ExpectedCombatHistoryEventInterval = 10.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|History", meta=(EditCondition="bKeepCombatObservationHistory"))
	int CombatHistorySize = 10;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|LIDAR")
	FRaindropParams RaindropParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|LIDAR")
	FConv2dObservationParams LidarCommonConv2DParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|LIDAR")
	FConv2dObservationParams LidarRaindropDownwardConv2dParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|LIDAR")
	FConv2dObservationParams LidarRaindropForwardConv2dParams;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|LIDAR")
	FConv2dObservationParams LidarRaindropBackwardConv2dParams;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category="Observations|LIDAR")
	TMap<ELAAgentAttitude, FConv2dObservationParams> LidarRaindropToTargetConv2dParams;
	
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
	TMap<FGameplayTag, float> Consumables;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config)
	bool bVisLogEnabled = true;
};

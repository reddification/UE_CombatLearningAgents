// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/AgentCombatDataTypes.h"
#include "Data/LearningAgentsDataTypes.h"
#include "Settings/CombatLearningSettings.h"
#include "LearningAgentCombatObservationComponent.generated.h"

namespace CombatLearning
{
	struct FSelfData;
}

using namespace CombatLearning;

UCLASS(Abstract)
class NPC_ML_API ULearningAgentCombatObservationComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	struct FLidarRaindropVariables
	{
		std::atomic<bool> bActive { false };
		mutable FCriticalSection CriticalSection;
		FVector OriginVector;
		FVector DirectionVectorX;
		FVector DirectionVectorY;
		FVector TraceDirection;
	}; 
	
public:
	ULearningAgentCombatObservationComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual FSelfData GetSelfData() const;
	virtual FCombatStateData GetCombatStateData() const;
	virtual TArray<FEnemyData> GetEnemies() const;
	virtual TArray<FAllyData> GetAllies() const;
	
	const FLidarObservationCache& GetLidarData() const { return LidarObservationCache; }
	
	void OnCombatStarted();
	void OnCombatEnded();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<ECollisionChannel> RaindropCollisionChannel;

	virtual float GetNormalizedStamina() const { return 1.f; }
	virtual float GetNormalizedHealth() const { return 1.f; }
	virtual float GetArmorRate() const { return 0.f; }
	virtual ELACharacterStates GetCombatStates() const { return ELACharacterStates::None; }
	virtual int GetLevel() const { return 1; }
	virtual float GetSurvivalDesire() const { return 0.75f; }
	virtual FWeaponData GetWeaponData() const { return {}; }
	virtual FGameplayTag GetActiveGesture(AActor* Target) const { return FGameplayTag::EmptyTag; }

	std::atomic<bool> LidarCancellationToken = false;
	
private:
	float CombatStartTime = 0.f;
	
	void CollectSpatialObservation_Octree();

	void LidarRaindropAsync(const FLidarRaindropVariables& RaindropVariables,
	                        const FLidarRaindropParams& RaindropParams, float* RawData, std::atomic<bool>* bActiveFlag, int RaindropLogId);
	void Raindrop(const FLidarRaindropVariables& RaindropVariables, const FLidarRaindropParams& RaindropParams, 
		float* RawData, int RaindropResolution, int x) const;
	bool IsAsyncRaindropActive() const;
	
	FLidarObservationCache LidarObservationCache;
	FLidarRaindropVariables RaindropDownwardsVariables;
	FLidarRaindropVariables RaindropForwardVariables;
	FLidarRaindropVariables RaindropBackwardsVariables;
	
	TWeakObjectPtr<const UCombatLearningSettings> Settings;
};

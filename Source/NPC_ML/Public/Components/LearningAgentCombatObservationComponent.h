// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/AgentCombatDataTypes.h"
#include "Data/InternalObservationDataTypes.h"
#include "Data/LearningAgentsDataTypes.h"
#include "Settings/CombatLearningSettings.h"
#include "Data/RaindropDataTypes.h"
#include "LearningAgentCombatObservationComponent.generated.h"

namespace CombatLearning
{
	struct FSelfData;
}

using namespace CombatLearning;

/*
 * Put a child component of this component on your agent actor
 */
UCLASS(Abstract)
class NPC_ML_API ULearningAgentCombatObservationComponent : public UActorComponent
{
	GENERATED_BODY()

	using FCharacterDataContainer = TMap<TWeakObjectPtr<AActor>, TSharedPtr<FOtherCharacterObservationData>>;
	
public:
	ULearningAgentCombatObservationComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual FSelfData GetSelfData() const;
	virtual FCombatStateData GetCombatStateData() const;
	bool OccupyRaindropBuffer(ELARaindropTarget RaindropTarget, FRaindropBufferHandle& Handle);

	const FCharacterDataContainer& GetEnemiesObservationData();
	const FCharacterDataContainer& GetAlliesObservationData();
	
	void OnCombatStarted();
	void OnCombatEnded();
	
	bool HasRelevantLidarData(AActor* Actor, ELARaindropTarget TargetType) const;
	const TArray<float>* GetLidarDataTo(AActor* ForActor, ELARaindropTarget TargetType) const;
	FORCEINLINE int GetRaindropToTargetResolution(ELARaindropTarget Target) const { return Settings->RaindropParams[Target].GetResolution(); }
	const FLidarSelfObservationCache& GetSelfLidarData() { return LidarSelfObservationCache; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual float GetNormalizedStamina() const { return 1.f; }
	virtual float GetNormalizedHealth() const { return 1.f; }
	virtual float GetArmorRate() const { return 0.f; }
	virtual ELACharacterStates GetCombatStates() const { return ELACharacterStates::None; }
	virtual int GetLevel() const { return 1; }
	virtual float GetSurvivalDesire() const { return 0.75f; }
	virtual FWeaponData GetWeaponData() const { return {}; }
	virtual FGameplayTag GetActiveGesture(AActor* Target) const { return FGameplayTag::EmptyTag; }

	virtual TArray<TSharedRef<FOtherCharacterState>> GetEnemies() const;
	virtual TArray<TSharedRef<FOtherCharacterState>> GetAllies() const;
	
private:
	float CombatStartTime = 0.f;
	
	std::atomic<bool> LidarCancellationToken = false;
	std::atomic<int> ActiveRaindropsCount { 0 };
	
	void CollectSpatialObservation_Octree();

	void ProcessTargetObservations(TArray<TSharedRef<FOtherCharacterState>>&& Targets, ELARaindropTarget RaindropTarget);
	void LidarRaindropAsync(const FLidarRaindropVariables* RaindropVariables, const FLidarRaindropParams* RaindropParams, const
	                        FRaindropBufferHandle* RaindropBufferHandle, ELARaindropTarget
	                        TargetType);
	void RaindropRow(const FLidarRaindropVariables* RaindropVariables, const FLidarRaindropParams* RaindropParams,
	              const FRaindropRowData& RowData);
	void RaindropToArray(const FLidarRaindropVariables* RaindropVariables, const FLidarRaindropParams* RaindropParams,
		const FRaindropRowData& RowData, TArray<float>& Array) const;
	bool IsAsyncRaindropActive() const;

	bool IsCharacterRelevantForRaindrop(const FOtherCharacterState& CharacterState, const FVector& LastKnownLocation,
	                                    const FRaindropRelevancyParams& RelevancyParams) const;
	void UpdateRaindropsToTargets(FCharacterDataContainer& CharactersData, ELARaindropTarget TargetType);
	void StopRaindrop(FRaindrop& RaindropData, ELARaindropTarget Attitude);
	void ResetRaindropBuffers();

	TMap<ELARaindropTarget, TArray<FRaindropBuffer>> RaindropBuffers;
	FCharacterDataContainer CachedEnemiesData;
	FCharacterDataContainer CachedAlliesData;

	FLidarSelfObservationCache LidarSelfObservationCache;
	
	FRaindrop SelfDownwardRaindrop;
	FRaindrop SelfForwardRaindrop;
	FRaindrop SelfBackwardRaindrop;
	
	TWeakObjectPtr<const UCombatLearningSettings> Settings;
	FTimerHandle ResetRaindropBuffersTimer;
};

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
	bool OccupyRaindropBuffer(ELAAgentAttitude RaindropTarget, FRaindropBufferHandle& Handle);

	const FCharacterDataContainer& GetEnemiesObservationData();
	const FCharacterDataContainer& GetAlliesObservationData();
	
	void OnCombatStarted();
	void OnCombatEnded();
	
	bool HasRelevantLidarData(AActor* Actor, ELAAgentAttitude TargetType) const;
	const TArray<float>* GetLidarDataTo(AActor* ForActor, ELAAgentAttitude TargetType) const;
	FORCEINLINE int GetRaindropToTargetResolution(ELAAgentAttitude Target) const { return Settings->RaindropParams[Target].GetResolution(); }
	const FLidarSelfObservationCache& GetSelfLidarData() const { return LidarSelfObservationCache; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual float GetNormalizedHealth(AActor* ForActor) const { return 1.f; }
	virtual float GetNormalizedStamina(AActor* ForActor) const { return 1.f; }
	virtual float GetArmorRate(AActor* ForActor) const { return 0.f; }
	virtual ELACharacterStates GetCombatStates(AActor* ForActor) const { return ELACharacterStates::None; }
	virtual int GetLevel(AActor* ForActor) const { return 1; }
	virtual float GetSurvivalDesire() const { return 0.75f; }
	virtual FWeaponData GetWeaponData(AActor* ForActor) const { return {}; }
	virtual const FGameplayTag& GetActiveGesture(AActor* ForActor) const { return FGameplayTag::EmptyTag; }
	virtual const FGameplayTag& GetActivePhrase(AActor* ForActor) const { return FGameplayTag::EmptyTag; }
	virtual float GetAccumulatedNormalizedDamage(AActor* ForActor) const { return 0.f; }
	
	virtual TArray<TSharedRef<FPerceivedCharacterData>> GetEnemies() const;
	virtual TArray<TSharedRef<FPerceivedCharacterData>> GetAllies() const;
	
private:
	float CombatStartTime = 0.f;
	
	std::atomic<bool> LidarCancellationToken = false;
	std::atomic<int> ActiveRaindropsCount { 0 };
	
	void CollectSpatialObservation_Octree();

	void ProcessTargetObservations(TArray<TSharedRef<FPerceivedCharacterData>>&& Targets, ELAAgentAttitude RaindropTarget);
	void LidarRaindropAsync(const FLidarRaindropVariables* RaindropVariables, const FLidarRaindropParams* RaindropParams, 
		const FRaindropBufferHandle* RaindropBufferHandle, ELAAgentAttitude TargetType);
	void RaindropRow(const FLidarRaindropVariables* RaindropVariables, const FLidarRaindropParams* RaindropParams, const FRaindropRowData& RowData);
	void RaindropToArray(const FLidarRaindropVariables* RaindropVariables, const FLidarRaindropParams* RaindropParams,
		const FRaindropRowData& RowData, TArray<float>& Array) const;
	bool IsAsyncRaindropActive() const;

	bool IsCharacterRelevantForRaindrop(const FPerceivedCharacterData& CharacterState, const FVector& LastKnownLocation, const FRaindropRelevancyParams& RelevancyParams) const;
	void UpdateRaindropsToTargets(FCharacterDataContainer& CharactersData, ELAAgentAttitude TargetType);
	void StopRaindrop(FRaindrop& RaindropData, ELAAgentAttitude Attitude);
	void ResetRaindropBuffers();

	TMap<ELAAgentAttitude, TArray<FRaindropBuffer>> RaindropBuffers;
	FCharacterDataContainer CachedEnemiesData;
	FCharacterDataContainer CachedAlliesData;

	FLidarSelfObservationCache LidarSelfObservationCache;
	
	FRaindrop SelfDownwardRaindrop;
	FRaindrop SelfForwardRaindrop;
	FRaindrop SelfBackwardRaindrop;
	
	TWeakObjectPtr<const UCombatLearningSettings> Settings;
	FTimerHandle ResetRaindropBuffersTimer;
};

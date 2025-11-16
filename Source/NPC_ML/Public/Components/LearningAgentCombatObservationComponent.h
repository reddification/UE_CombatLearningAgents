// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/AgentCombatDataTypes.h"
#include "Data/LearningAgentsDataTypes.h"
#include "LearningAgentCombatObservationComponent.generated.h"

namespace CombatLearning
{
	struct FSelfData;
}

using namespace CombatLearning;

/*
 * Put a child component on your agent actor and override all relevant methods
 */
UCLASS(Abstract)
class NPC_ML_API ULearningAgentCombatObservationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULearningAgentCombatObservationComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual FSelfData GetSelfData() const;
	virtual FCombatStateData GetCombatStateData() const;
	virtual TArray<FEnemyData> GetEnemies() const;
	virtual TArray<FAllyData> GetAllies() const;

	void OnCombatStarted();
	void OnCombatEnded();
	TArray<FLAWalkablePath> GetWalkablePaths() const;
	TArray<FLASpatialObservation> GetSpatialData();

protected:
	virtual float GetNormalizedStamina() const { return 1.f; }
	virtual float GetNormalizedHealth() const { return 1.f; }
	virtual float GetArmorRate() const { return 0.f; }
	virtual ELACombatState GetCombatStates() const { return ELACombatState::None; }
	virtual int GetLevel() const { return 1; }
	virtual float GetSurvivalDesire() const { return 0.75f; }
	virtual FWeaponData GetWeaponData() const { return {}; }
	virtual ELAGesture GetActiveGesture(AActor* Target) const { return ELAGesture::None; }

private:
	float CombatStartTime = 0.f;
};

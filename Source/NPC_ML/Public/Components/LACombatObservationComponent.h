// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/AgentCombatDataTypes.h"
#include "Data/LearningAgentsDataTypes.h"
#include "Settings/CombatLearningSettings.h"
#include "LACombatObservationComponent.generated.h"

namespace CombatLearning
{
	struct FSelfData;
}

using namespace CombatLearning;
using FCharacterDataContainer = TArray<TSharedRef<FPerceivedCharacterData>>;

/*
 * Put a child component of this component on your agent actor
 */
UCLASS(Abstract)
class NPC_ML_API ULACombatObservationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULACombatObservationComponent();

	virtual FSelfData GetSelfData() const;
	virtual FCombatStateData GetCombatStateData() const;

	FCharacterDataContainer GetEnemiesObservationData();
	FCharacterDataContainer GetAlliesObservationData();
	
	void OnCombatStarted();
	void OnCombatEnded();
	
protected:
	virtual void BeginPlay() override;
	
	virtual float GetNormalizedHealth(const AActor* ForActor) const { return 1.f; }
	virtual float GetNormalizedStamina(const AActor* ForActor) const { return 1.f; }
	virtual float GetNormalizedPoise(const AActor* ForActor) const { return 1.f; };
	virtual float GetArmorRate(const AActor* ForActor) const { return 0.f; }
	virtual ELACharacterStates GetCombatStates(const AActor* ForActor) const { return LACharacterStates::None; }
	virtual FWeaponData GetWeaponData(const AActor* ForActor) const { return {}; }
	virtual const FGameplayTag& GetActiveGesture(const AActor* ForActor) const { return FGameplayTag::EmptyTag; }
	virtual const FGameplayTag& GetActivePhrase(const AActor* ForActor) const { return FGameplayTag::EmptyTag; }
	virtual float GetAccumulatedNormalizedDamage(const AActor* ForActor) const { return 0.f; }
	virtual FRoleplayIdentity GetIdentity(const AActor* ForActor) const { return FRoleplayIdentity(); };
	virtual uint8 GetActiveAttack(AActor* Actor) const { return 0; };
	
	virtual TArray<TSharedRef<FPerceivedCharacterData>> GetEnemies() const;
	virtual TArray<TSharedRef<FPerceivedCharacterData>> GetAllies() const;

private:
	float CombatStartTime = 0.f;
	
	void ProcessTargetObservations(TArray<TSharedRef<FPerceivedCharacterData>>& Targets, ELAAgentAttitude AgentAttitude);


	TWeakObjectPtr<const UCombatLearningSettings> Settings;
};

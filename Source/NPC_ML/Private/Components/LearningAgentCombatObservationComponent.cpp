#include "Components/LearningAgentCombatObservationComponent.h"

#include "NavigationSystem.h"
#include "Data/AgentCombatDataTypes.h"

ULearningAgentCombatObservationComponent::ULearningAgentCombatObservationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void ULearningAgentCombatObservationComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FSelfData ULearningAgentCombatObservationComponent::GetSelfData() const
{
	FSelfData SelfData;

	auto OwnerActor = GetOwner();
	SelfData.NormalizedHealth = GetNormalizedHealth();
	SelfData.NormalizedStamina = GetNormalizedStamina();
	SelfData.ArmorRate = GetArmorRate();
	SelfData.CombatStates = GetCombatStates();
	SelfData.Level = GetLevel();
	SelfData.SurvivalDesire = GetSurvivalDesire();
	SelfData.WeaponData = GetWeaponData();
	SelfData.WorldVelocity = OwnerActor->GetVelocity();
	SelfData.ActiveGesture = GetActiveGesture(OwnerActor);
	
	return SelfData;
}

FCombatStateData ULearningAgentCombatObservationComponent::GetCombatStateData() const
{
	FCombatStateData Result;
	Result.CombatTotalDuration = GetWorld()->GetTimeSeconds() - CombatStartTime;
	return Result;
}

TArray<FEnemyData> ULearningAgentCombatObservationComponent::GetEnemies() const
{
	return {};
}

TArray<FAllyData> ULearningAgentCombatObservationComponent::GetAllies() const
{
	return {};
}

void ULearningAgentCombatObservationComponent::OnCombatStarted()
{
	CombatStartTime = GetWorld()->GetTimeSeconds();
}

void ULearningAgentCombatObservationComponent::OnCombatEnded()
{
	
}

TArray<FLAWalkablePath> ULearningAgentCombatObservationComponent::GetWalkablePaths() const
{
	return {};
}

TArray<FLASpatialObservation> ULearningAgentCombatObservationComponent::GetSpatialData()
{
	return {};
}

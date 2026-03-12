#include "Components/LACombatObservationComponent.h"

#include "Data/AgentCombatDataTypes.h"
#include "Settings/CombatLearningSettings.h"

ULACombatObservationComponent::ULACombatObservationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void ULACombatObservationComponent::BeginPlay()
{
	Super::BeginPlay();
	Settings = GetDefault<UCombatLearningSettings>();
}

FSelfData ULACombatObservationComponent::GetSelfData() const
{
	FSelfData SelfData;

	auto OwnerActor = GetOwner();
	SelfData.NormalizedHealth = GetNormalizedHealth(OwnerActor);
	SelfData.Actor = OwnerActor;
	SelfData.NormalizedStamina = GetNormalizedStamina(OwnerActor);
	SelfData.NormalizedPoise = GetNormalizedPoise(OwnerActor);
	SelfData.ArmorRate = GetArmorRate(OwnerActor);
	SelfData.CombatStates = GetCombatStates(OwnerActor);
	SelfData.WeaponData = GetWeaponData(OwnerActor);
	SelfData.WorldVelocity = OwnerActor->GetVelocity();
	SelfData.ActiveGesture = GetActiveGesture(OwnerActor);
	SelfData.ActivePhrase = GetActivePhrase(OwnerActor);
	SelfData.AccumulatedNormalizedDamage = GetAccumulatedNormalizedDamage(OwnerActor);
	SelfData.Identity = GetIdentity(OwnerActor);
	SelfData.ActiveAttack = GetActiveAttack(OwnerActor);
	return SelfData;
}

FCombatStateData ULACombatObservationComponent::GetCombatStateData() const
{
	FCombatStateData Result;
	Result.CombatTotalDuration = GetWorld()->GetTimeSeconds() - CombatStartTime;
	return Result;
}

void ULACombatObservationComponent::ProcessTargetObservations(TArray<TSharedRef<FPerceivedCharacterData>>& Targets, ELAAgentAttitude AgentAttitude)
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	int MaxCountOfTargets = 0;
	FCharacterDataContainer* CachedTargetsData = nullptr;
	switch (AgentAttitude)
	{
		case ELAAgentAttitude::Enemy:
			MaxCountOfTargets = Settings->MaxEnemiesObservedAtOnce;
			break;
		case ELAAgentAttitude::Ally:
			MaxCountOfTargets = Settings->MaxAlliesObservedAtOnce;
			break;
		default:
			ensure(false);
		break;
	}
	
	if (CachedTargetsData == nullptr)
		return;

	// sort and shrink enemies
	Targets.Sort([OwnerLocation](const TSharedPtr<FPerceivedCharacterData>& A, const TSharedPtr<FPerceivedCharacterData>& B)
	{
		if (!A->bAgentCanSeeCharacter && B->bAgentCanSeeCharacter || !A->bAlive && B->bAlive)
			return false;
		
		if (A->bAgentCanSeeCharacter && !B->bAgentCanSeeCharacter || A->bAlive && !B->bAlive)
			return true;
			
		return (A->Actor->GetActorLocation() - OwnerLocation).SizeSquared() < (B->Actor->GetActorLocation() - OwnerLocation).SizeSquared();
	});
	
	if (Targets.Num() > MaxCountOfTargets)
		Targets.RemoveAt(MaxCountOfTargets, Targets.Num() - MaxCountOfTargets);
	
	// remove those that don't present anymore
	TSet<const AActor*> TargetActorsSet;
	TargetActorsSet.Reserve(Targets.Num());
	for (int i = 0; i < Targets.Num(); i++)
		TargetActorsSet.Add(Targets[i]->Actor.Get());
}

FCharacterDataContainer ULACombatObservationComponent::GetEnemiesObservationData()
{
	TArray<TSharedRef<FPerceivedCharacterData>> Enemies = GetEnemies();
	ProcessTargetObservations(Enemies, ELAAgentAttitude::Enemy);
	return Enemies;
}

FCharacterDataContainer ULACombatObservationComponent::GetAlliesObservationData()
{
	TArray<TSharedRef<FPerceivedCharacterData>> Allies = GetAllies();
	ProcessTargetObservations(Allies, ELAAgentAttitude::Ally);
	return Allies;
}

TArray<TSharedRef<FPerceivedCharacterData>> ULACombatObservationComponent::GetEnemies() const
{
	unimplemented();
	return {};
}

TArray<TSharedRef<FPerceivedCharacterData>> ULACombatObservationComponent::GetAllies() const
{
	unimplemented();
	return {};
}

void ULACombatObservationComponent::OnCombatStarted()
{
	CombatStartTime = GetWorld()->GetTimeSeconds();
	// SetComponentTickEnabled(true);
}

void ULACombatObservationComponent::OnCombatEnded()
{
	// SetComponentTickEnabled(false);
}
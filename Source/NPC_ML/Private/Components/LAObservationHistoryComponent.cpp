// 


#include "Components/LAObservationHistoryComponent.h"

#include "Data/LearningAgentsDataTypes.h"
#include "Settings/CombatLearningSettings.h"
#include "Subsystems/LAObservationHistorySubsystem.h"

void ULAObservationHistoryComponent::BeginPlay()
{
	Super::BeginPlay();
	Settings = GetDefault<UCombatLearningSettings>();
	if (!Settings->bKeepCombatObservationHistory)
		return;
	
	OwnerPawn = Cast<APawn>(GetOwner());
	
	FCombatEventData DefaultCombatEvent(0.f, ELACombatEvent::None, true, ELAAgentAttitude::Self);
	CombatHistory.SetNumUninitialized(Settings->CombatHistorySize);
	for (int i = 0; i < Settings->CombatHistorySize; i++)
		CombatHistory[i] = DefaultCombatEvent;
	
	ObservationHistorySubsystem = GetWorld()->GetSubsystem<ULAObservationHistorySubsystem>();
}

void ULAObservationHistoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ObservationHistorySubsystem.IsValid() && IsValid(ObservationHistorySubsystem.Get()))
		ObservationHistorySubsystem->UnregisterAgent(OwnerPawn.Get());
	
	Super::EndPlay(EndPlayReason);
}

void ULAObservationHistoryComponent::AddCombatEvent(ELACombatEvent CombatEvent, bool bEventSubject, ELAAgentAttitude AttitudeToCauser)
{
	if (!bActive)
		return;
	
	float CurrentTime = GetWorld()->GetTimeSeconds();
	CombatHistory[CombatHistoryIndex] = FCombatEventData(CurrentTime, CombatEvent, bEventSubject, AttitudeToCauser);
	CombatHistoryIndex = (CombatHistoryIndex + 1) % CombatHistory.Num();
}

TArray<FCombatEventData> ULAObservationHistoryComponent::GetCombatHistory() const
{
	int EventIndex = CombatHistoryIndex;
	TArray<FCombatEventData> Result;
	Result.Reserve(CombatHistory.Num());
	// order doesn't really matter for ML as long as it is consistent
	// in this case the order is: from the oldest event to the newest
	for (int i = 0; i < CombatHistory.Num(); i++)
	{
		Result.Add(CombatHistory[EventIndex]);
		EventIndex = (EventIndex + 1) % CombatHistory.Num();
	}
	
	return Result;
}

TArray<FTranslationHistory> ULAObservationHistoryComponent::GetTranslationHistory(AActor* ForActor) const
{
	return ObservationHistorySubsystem->GetTranslationHistory(ForActor, OwnerPawn.Get());
}

void ULAObservationHistoryComponent::SetHistoryActive(bool bNewActive)
{
	Settings = GetDefault<UCombatLearningSettings>();
	if (!Settings->bKeepCombatObservationHistory)
		return;
	
	if (bNewActive == bActive)
		return;
	
	bActive = bNewActive;
	if (bActive)
		ObservationHistorySubsystem->RegisterAgent(OwnerPawn.Get());
	else 
		ObservationHistorySubsystem->UnregisterAgent(OwnerPawn.Get());
}

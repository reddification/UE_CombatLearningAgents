// 


#include "Subsystems/LAObservationHistorySubsystem.h"

#include "Data/LearningAgentsDataTypes.h"
#include "Particles/FXBudget.h"
#include "Settings/CombatLearningSettings.h"

void ULAObservationHistorySubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	const auto Settings = GetDefault<UCombatLearningSettings>();
	TranslationHistorySize = Settings->TranslationHistorySize;
	UpdateInterval = Settings->TranslationHistoryUpdateInterval;
}

void ULAObservationHistorySubsystem::OnWorldEndPlay(UWorld& InWorld)
{
	InWorld.GetTimerManager().ClearTimer(UpdateTranslationsTimer);
	Super::OnWorldEndPlay(InWorld);
}

void ULAObservationHistorySubsystem::RegisterAgent(AActor* Agent)
{
	if (!ensure(!TranslationHistories.Contains(Agent)))
		return;
	
	TArray<FTranslationHistory> TranslationHistory;
	TranslationHistory.SetNumUninitialized(TranslationHistorySize);
	FTransform InitialTransform = Agent->GetTransform();
	for (int i = 0; i < TranslationHistorySize; i++)
		TranslationHistory[i] = FTranslationHistory(FVector::ZeroVector, InitialTransform);
	
	TranslationHistories.Add(Agent, TranslationHistory);
	RegisterHistoryConsumer();
}

void ULAObservationHistorySubsystem::UnregisterAgent(AActor* Agent)
{
	if (TranslationHistories.Contains(Agent))
	{
		TranslationHistories.Remove(Agent);
		UnregisterHistoryConsumer();
	}
}

void ULAObservationHistorySubsystem::RegisterHistoryConsumer()
{
	ConsumersCount++;
	ensure(ConsumersCount >= 0);
	if (ConsumersCount == 1)
	{
		auto& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.SetTimer(UpdateTranslationsTimer, this, &ULAObservationHistorySubsystem::UpdateTranslationHistories,UpdateInterval, true);
	}
}

void ULAObservationHistorySubsystem::UnregisterHistoryConsumer()
{
	ConsumersCount--;
	ensure(ConsumersCount >= 0);
	if (ConsumersCount == 0)
		GetWorld()->GetTimerManager().ClearTimer(UpdateTranslationsTimer);
}

TArray<FTranslationHistory> ULAObservationHistorySubsystem::GetTranslationHistory(AActor* ForAgent, AActor* ObservedByAgent) const
{
	if (!TranslationHistories.Contains(ForAgent))
		return {};
	
	bool bRelativeCase = ObservedByAgent != nullptr && ObservedByAgent != ForAgent;
	if (bRelativeCase)
		if (!ensure(TranslationHistories.Contains(ObservedByAgent)))
			return {};
	
	const TArray<FTranslationHistory>& OriginTransformHistory = bRelativeCase ? TranslationHistories[ObservedByAgent] : TranslationHistories[ForAgent];
	
	int EventIndex = TranslationHistoryRecordIndex;
	const TArray<FTranslationHistory>& SourceTranslationHistory = TranslationHistories[ForAgent];
	TArray<FTranslationHistory> Result;
	Result.SetNumUninitialized(TranslationHistorySize);
	for (int i = 0; i < TranslationHistorySize; i++)
	{
		Result[i] = SourceTranslationHistory[EventIndex];
		Result[i].RelativeTransform = OriginTransformHistory[EventIndex].Transform;
		EventIndex = (EventIndex + 1) % TranslationHistorySize;
	}
	
	return Result;
}

void ULAObservationHistorySubsystem::UpdateTranslationHistories()
{
	for (auto& TranslationHistory : TranslationHistories)
		if (ensure(TranslationHistory.Key.IsValid()))
			TranslationHistory.Value[TranslationHistoryRecordIndex] = FTranslationHistory(TranslationHistory.Key.Get());
	
	TranslationHistoryRecordIndex = (TranslationHistoryRecordIndex + 1) % TranslationHistorySize;
}

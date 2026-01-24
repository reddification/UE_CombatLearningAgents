// 

#pragma once

#include "CoreMinimal.h"
#include "Settings/CombatLearningSettings.h"
#include "Subsystems/WorldSubsystem.h"
#include "LAObservationHistorySubsystem.generated.h"

struct FTranslationHistory;
/**
 * Translation history recording is only active when there's at least one consumer/provider of history 
 * These consumers/providers must be registered by calling RegisterAgent/UnregisterAgent
 * Typically you want agents to register when they enter combat and actually require ML inference
 * Otherwise there's no reason to maintain translation history for insignificant actors
 */
UCLASS()
class NPC_ML_API ULAObservationHistorySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	void RegisterAgent(AActor* Agent);
	void UnregisterAgent(AActor* Agent);
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void OnWorldEndPlay(UWorld& InWorld) override;
	
	TArray<FTranslationHistory> GetTranslationHistory(AActor* ForAgent, AActor* ObservedByAgent) const;
	
private:
	void UpdateTranslationHistories();
	void RegisterHistoryConsumer();
	void UnregisterHistoryConsumer();

	FTimerHandle UpdateTranslationsTimer;
	TMap<TWeakObjectPtr<AActor>, TArray<FTranslationHistory>> TranslationHistories;
	int TranslationHistoryRecordIndex = 0;

	int ConsumersCount = 0;
	int TranslationHistorySize = 0;
	float UpdateInterval = 0.1f;
};

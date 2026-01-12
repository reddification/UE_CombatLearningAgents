// 

#pragma once

#include "CoreMinimal.h"
#include "Settings/CombatLearningSettings.h"
#include "Subsystems/WorldSubsystem.h"
#include "LAObservationHistorySubsystem.generated.h"

struct FTranslationHistory;
/**
 * 
 */
UCLASS()
class NPC_ML_API ULAObservationHistorySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	void RegisterAgent(AActor* Agent);
	void UnregisterAgent(AActor* Agent);
	void SetRecordingEnabled(bool bEnabled);

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void OnWorldEndPlay(UWorld& InWorld) override;
	
	TArray<FTranslationHistory> GetTranslationHistory(AActor* ForAgent, AActor* ObservedByAgent) const;
	
private:
	void UpdateTranslationHistories();
	
	FTimerHandle UpdateTranslationsTimer;
	TMap<TWeakObjectPtr<AActor>, TArray<FTranslationHistory>> TranslationHistories;
	int TranslationHistoryRecordIndex = 0;
	
	int TranslationHistorySize = 0;
	float UpdateInterval = 0.1f;
};

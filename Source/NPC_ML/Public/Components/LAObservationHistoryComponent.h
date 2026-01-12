// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Subsystems/LAObservationHistorySubsystem.h"
#include "LAObservationHistoryComponent.generated.h"


class UCombatLearningSettings;
enum class ELACombatEvent : uint8;
struct FCombatEventData;
struct FTranslationHistory;

/*
 * This class is necessary to substitute for limitations of Feed-Forward Neural Networks that are used in the current UE ML framework implementation
 * That is - lack of memory, or awareness of previous observations
 * This component must be placed on all agent-relevant entities: both players and other agents
 * 
 * You must populate combat history in your project implementation
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NPC_ML_API ULAObservationHistoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void AddCombatEvent(ELACombatEvent CombatEvent);
	TArray<FCombatEventData> GetCombatHistory() const;
	
	TArray<FTranslationHistory> GetTranslationHistory(AActor* ForActor) const;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	TWeakObjectPtr<APawn> OwnerPawn;
	TWeakObjectPtr<const UCombatLearningSettings> Settings;
	TWeakObjectPtr<ULAObservationHistorySubsystem> ObservationHistorySubsystem;
	
	TArray<FCombatEventData> CombatHistory;
	
	int CombatHistoryIndex = 0;
};

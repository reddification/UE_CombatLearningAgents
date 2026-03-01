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
 *
*		============ OLD DESCRIPTION =================
 * This class is necessary to substitute for limitations of Feed-Forward Neural Networks that are used in the current UE ML framework implementation
 * That is - lack of memory, or awareness of previous observations
 * This component must be placed on all agent-relevant entities: both players and other agents
 * 
 * You must populate combat history in your project implementation
 * You must activate this component when needed (@see SetHistoryActive)
 *		I mean, you can do it in BeginPlay if you want,
 *		but that would also affect translation history accumulation, which might be redundant to do for dozens of NPCs which may not even participate in combat
 *		
 *		============= NEW DESCRIPTION (1 Mar 2026) =================
 *	It seems that I was wrong and UE LA framework can now actually train and infere RNNs (or something close to it)
 *	In Policy settings FLearningAgentsPolicySettings, there are property "memory cell type" and "memory state size"
 *	So this component PROBABLY is redundant now. I will PROBABLY remove it when and if I am sattisfied with training/inference without it 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NPC_ML_API ULAObservationHistoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void AddCombatEvent(ELACombatEvent CombatEvent, bool bEventSubject, ELAAgentAttitude AttitudeToCauser);
	TArray<FCombatEventData> GetCombatHistory() const;
	
	TArray<FTranslationHistory> GetTranslationHistory(AActor* ForActor) const;

	void SetHistoryActive(bool bActive);
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	TWeakObjectPtr<APawn> OwnerPawn;
	TWeakObjectPtr<const UCombatLearningSettings> Settings;
	TWeakObjectPtr<ULAObservationHistorySubsystem> ObservationHistorySubsystem;
	
	TArray<FCombatEventData> CombatHistory;
	
	int CombatHistoryIndex = 0;
	bool bActive = false;
};

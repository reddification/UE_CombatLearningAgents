// 

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsInteractor.h"
#include "Components/LearningAgentCombatObservationComponent.h"
#include "Settings/CombatLearningSettings.h"
#include "LearningAgentsInteractor_Combat.generated.h"

/**
 * 
 */
UCLASS()
class NPC_ML_API ULearningAgentsInteractor_Combat : public ULearningAgentsInteractor
{
	GENERATED_BODY()

public:
	virtual void SpecifyAgentObservation_Implementation(FLearningAgentsObservationSchemaElement& OutObservationSchemaElement,
		ULearningAgentsObservationSchema* InObservationSchema) override;
	virtual void GatherAgentObservation_Implementation(FLearningAgentsObservationObjectElement& OutObservationObjectElement, 
	                                                   ULearningAgentsObservationObject* InObservationObject, const int32 AgentId) override;
	
	virtual void SpecifyAgentAction_Implementation(FLearningAgentsActionSchemaElement& OutActionSchemaElement,
		ULearningAgentsActionSchema* InActionSchema) override;
	virtual void MakeAgentActionModifier_Implementation(FLearningAgentsActionModifierElement& OutActionModifierElement, 
	                                                    ULearningAgentsActionModifier* InActionModifier, const ULearningAgentsObservationObject* InObservationObject,
	                                                    const FLearningAgentsObservationObjectElement& InObservationObjectElement, const int32 AgentId) override;
	virtual void PerformAgentAction_Implementation(const ULearningAgentsActionObject* InActionObject,
		const FLearningAgentsActionObjectElement& InActionObjectElement, const int32 AgentId) override;
	
protected:
	virtual TMap<FName, FLearningAgentsObservationSchemaElement> GetSelfObservationsSchema(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const;
	virtual TMap<FName, FLearningAgentsObservationSchemaElement> GetLidarObservationsSchema(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const;
	virtual FLearningAgentsObservationSchemaElement GetEnemyObservationSchema(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const;
	virtual FLearningAgentsObservationSchemaElement GetAllyObservationsSchema(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const;
	virtual TMap<FName, FLearningAgentsObservationSchemaElement> GetCombatStateObservationSchema(ULearningAgentsObservationSchema* InObservationSchema) const;

	virtual FLearningAgentsObservationObjectElement GatherSelfObservations(ULearningAgentsObservationObject* InObservationObject, int AgentId,
	                                                                       const FSelfData& SelfData) const;
	virtual FLearningAgentsObservationObjectElement GatherSurroundingsObservations(ULearningAgentsObservationObject* InObservationObject, int32 AgentId, ULearningAgentCombatObservationComponent*
		LAObservationComponent);
	virtual FLearningAgentsObservationObjectElement GatherCombatStateObservation(ULearningAgentsObservationObject* InObservationObject, int32 AgentId, const FCombatStateData& CombatStateData);
	virtual FLearningAgentsObservationObjectElement GatherEnemiesObservation(ULearningAgentsObservationObject* InObservationObject, int32 AgentId, const TArray<FEnemyData>& EnemiesData);
	virtual FLearningAgentsObservationObjectElement GatherAlliesObservations(ULearningAgentsObservationObject* InObservationObject, int32 AgentId, const TArray<FAllyData>& AlliesData);
	
private:
	FLearningAgentsObservationSchemaElement GetWeaponObservationSchema(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* LearningSettings) const;
	FLearningAgentsObservationObjectElement GetWeaponObservation(ULearningAgentsObservationObject* AgentObject, int
	                                                             AgentId, const FWeaponData& WeaponData, const UCombatLearningSettings* Settings, const FVector& AgentLocation) const;
	bool GetSelfStates(const ULearningAgentsObservationObject* InObservationObject,
					   const FLearningAgentsObservationObjectElement& InObservationObjectElement, ELACharacterStates& OutSelfStates) const;
	
	TSet<FName> GetMaskedActions(ELACharacterStates SelfStates) const;
	FLearningAgentsActionSchemaElement GetNamedOptionsAction(ULearningAgentsActionSchema* InActionSchema,
															 const FGameplayTagContainer& OptionTags, const FName& ActionTag) const;
};

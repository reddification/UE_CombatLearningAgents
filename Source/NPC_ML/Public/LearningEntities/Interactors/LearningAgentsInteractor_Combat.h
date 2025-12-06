// 

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsInteractor.h"
#include "Components/LearningAgentCombatActionsComponent.h"
#include "Components/LearningAgentCombatObservationComponent.h"
#include "Components/LearningAgentLocomotionActionsComponent.h"
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
	virtual FLearningAgentsObservationObjectElement GatherSurroundingsObservations(ULearningAgentsObservationObject* InObservationObject,
		int32 AgentId, ULearningAgentCombatObservationComponent* LAObservationComponent);
	virtual FLearningAgentsObservationObjectElement GatherCombatStateObservation(ULearningAgentsObservationObject* InObservationObject, 
		int32 AgentId, const FCombatStateData& CombatStateData);
	virtual FLearningAgentsObservationObjectElement GatherEnemiesObservation(ULearningAgentsObservationObject* InObservationObject, 
		int32 AgentId, const TArray<FEnemyData>& EnemiesData);
	virtual FLearningAgentsObservationObjectElement GatherAlliesObservations(ULearningAgentsObservationObject* InObservationObject, 
		int32 AgentId, const TArray<FAllyData>& AlliesData);
	
	virtual const UEnum* GetAttackEnum() { return StaticEnum<ELAAttackType>(); }
	virtual TMap<uint8, float> GetAttackEnumBaseProbabilities() const { return {{0, 1.f }}; }
	virtual TArray<uint8> GetMaskedAttackValues() const { return { }; }
	
private:
	FLearningAgentsObservationSchemaElement SpecifyNamedExclusiveDiscreteObservation(ULearningAgentsObservationSchema* InObservationSchema,
		const TMap<FGameplayTag, float>& ObservationOptions, const FName& ObservationName, const FName& ObservationOptionalWrapperName) const;
	FLearningAgentsObservationSchemaElement GetWeaponObservationSchema(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* LearningSettings) const;
	FLearningAgentsObservationObjectElement GetWeaponObservation(ULearningAgentsObservationObject* AgentObject, int
	                                                             AgentId, const FWeaponData& WeaponData, const UCombatLearningSettings* Settings, const FVector& AgentLocation) const;
	FLearningAgentsObservationObjectElement GatherOptionalNamedExclusiveObservation(ULearningAgentsObservationObject* InObservationObject,
		const FName& ObservationName, const FName& ObservationOptionalWrapperName, const FGameplayTag& OptionTag,
		const UCombatLearningSettings* Settings, const FVector& AgentWorldLocation, int AgentId) const;
	
	bool GetSelfStates(const ULearningAgentsObservationObject* InObservationObject,
					   const FLearningAgentsObservationObjectElement& InObservationObjectElement, ELACharacterStates& OutSelfStates) const;
	
	TSet<FName> GetMaskedActions(ELACharacterStates SelfStates) const;
	FLearningAgentsActionSchemaElement GetNamedOptionsActionSchemaElement(ULearningAgentsActionSchema* InActionSchema,
	                                                         const TMap<FGameplayTag, float>& Options, const FName& ActionTag) const;
	void SampleLocomotionAction(const ULearningAgentsActionObject* InActionObject, int32 AgentId, AActor* AgentActor,
		ULearningAgentLocomotionActionsComponent* LocomotionActionsComponent, const FLearningAgentsActionObjectElement& RootActionObjectElement);
	void SampleNonBlockingLocomotionActions(const ULearningAgentsActionObject* InActionObject, 
	                                        const TMap<FName, FLearningAgentsActionObjectElement>& NonBlockingLocomotionActionObjectElements, 
	                                        ULearningAgentLocomotionActionsComponent* LocomotionActionsComponent, int32 AgentId, const FVector& AgentLocation, const FTransform& AgentTransform, 
	                                        const UCombatLearningSettings* Settings);
	void SampleLocomotionAnimationAction(const ULearningAgentsActionObject* InActionObject, const FName& AnimationActionName,
										 const FLearningAgentsActionObjectElement& AnimationActionObjectElement,
										 ULearningAgentLocomotionActionsComponent* LocomotionActionsComponent,
										 int32 AgentId, const FVector& AgentLocation,
										 const UCombatLearningSettings* Settings);
	
	void SampleCombatAction(const ULearningAgentsActionObject* InActionObject, int32 AgentId, const AActor* AgentActor,
							ULearningAgentCombatActionsComponent* CombatActionsComponent,
							const FLearningAgentsActionObjectElement& RootActionObjectElement);
	TArray<uint8> GetBlockingLocomotionActionsMask(ELACharacterStates SelfStates) const;
	TArray<uint8> GetWeaponStateChangeMask(ELACharacterStates SelfStates) const;
	
};

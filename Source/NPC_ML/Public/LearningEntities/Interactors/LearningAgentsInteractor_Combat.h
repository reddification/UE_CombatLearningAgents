#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsInteractor.h"
#include "Components/LAObservationHistoryComponent.h"
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

private:
	struct FLAObservationGatherParams
	{
		ULearningAgentsObservationObject* InObservationObject = nullptr;
		const UCombatLearningSettings* Settings = nullptr;
		AActor* AgentActor = nullptr;
		FVector AgentWorldLocation = FVector::ZeroVector;
		FTransform AgentTransform = FTransform::Identity;
		int AgentId = 0;
	};
	
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
	virtual TMap<FName, FLearningAgentsObservationSchemaElement> SpecifySelfObservations(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const;
	virtual TMap<FName, FLearningAgentsObservationSchemaElement> SpecifyLidarSelfObservations(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const;
	virtual FLearningAgentsObservationSchemaElement SpecifyOtherCharacterDynamicObservations(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings,
		const TMap<FName, FLearningAgentsObservationSchemaElement>& AdditionalObservations, ELAAgentAttitude RaindropTarget) const;
	virtual FLearningAgentsObservationSchemaElement SpecifyEnemyObservation(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const;
	virtual FLearningAgentsObservationSchemaElement SpecifyAllyObservations(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const;
	virtual TMap<FName, FLearningAgentsObservationSchemaElement> SpecifyCombatStateObservation(ULearningAgentsObservationSchema* InObservationSchema) const;
	virtual FLearningAgentsObservationSchemaElement SpecifyCombatHistoryObservation(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const;
	virtual FLearningAgentsObservationSchemaElement SpecifyTranslationHistoryObservation(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const;
	
	virtual FLearningAgentsObservationObjectElement GatherSelfObservations(ULearningAgentsObservationObject* InObservationObject, int AgentId,
	                                                                       const FSelfData& SelfData);
	virtual FLearningAgentsObservationObjectElement GatherSurroundingsObservations(ULearningAgentsObservationObject* InObservationObject,
		int32 AgentId, ULearningAgentCombatObservationComponent* LAObservationComponent);
	virtual FLearningAgentsObservationObjectElement GatherCombatStateObservation(ULearningAgentsObservationObject* InObservationObject, 
		int32 AgentId, const FCombatStateData& CombatStateData);
	virtual FLearningAgentsObservationObjectElement GatherEnemiesObservation(ULearningAgentsObservationObject* InObservationObject,
	                                                                         ULearningAgentCombatObservationComponent*
	                                                                         CombatObservationComponent, int32 AgentId);
	virtual FLearningAgentsObservationObjectElement GatherAlliesObservations(ULearningAgentsObservationObject* InObservationObject,
	                                                                         ULearningAgentCombatObservationComponent*
	                                                                         CombatObservationComponent, int32 AgentId);
	
	virtual const UEnum* GetAttackEnum() { return StaticEnum<ELAAttackType>(); }
	virtual TMap<uint8, float> GetAttackEnumBaseProbabilities() const { return {{0, 1.f }}; }
	virtual TArray<uint8> GetMaskedAttackValues() const { return { }; }
	
private:
	FLearningAgentsObservationSchemaElement SpecifyNamedExclusiveDiscreteObservation(ULearningAgentsObservationSchema* InObservationSchema,
		const TMap<FGameplayTag, float>& ObservationOptions, const FName& ObservationName, const FName& ObservationOptionalWrapperName) const;
	FLearningAgentsObservationSchemaElement GetWeaponObservationSchema(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* LearningSettings) const;
	FLearningAgentsObservationObjectElement GetWeaponObservation(ULearningAgentsObservationObject* AgentObject, int AgentId, const FWeaponData& WeaponData,
		const UCombatLearningSettings* Settings, const FVector& AgentLocation);
	FLearningAgentsObservationSchemaElement SpecifyDynamicObservations(
		ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings,
		const TMap<FName, FLearningAgentsObservationSchemaElement>& ExtraObservations) const;
	FLearningAgentsObservationSchemaElement SpecifyStaticObservations(
		ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings, const TMap<FName,
		FLearningAgentsObservationSchemaElement>& AdditionalObservations) const;
	FLearningAgentsObservationObjectElement GatherOptionalNamedExclusiveObservation(ULearningAgentsObservationObject* InObservationObject,
		const FName& ObservationName, const FName& ObservationOptionalWrapperName, const FGameplayTag& OptionTag,
		const UCombatLearningSettings* Settings, const FVector& AgentWorldLocation, int AgentId);
	FLearningAgentsObservationObjectElement GatherStaticObservations(ULearningAgentsObservationObject* InObservationObject, int AgentId, const FCharacterDataBase& CharacterData,
	    const UCombatLearningSettings* Settings, const FVector& AgentWorldLocation, const TMap<FName, FLearningAgentsObservationObjectElement>& AdditionalObservations);
	FLearningAgentsObservationObjectElement GatherDynamicObservations(ULearningAgentsObservationObject* InObservationObject, int AgentId, const FCharacterDataBase& CharacterData,
		const UCombatLearningSettings* Settings, const TMap<FName, FLearningAgentsObservationObjectElement>& AdditionalObservations,
		const FVector& AgentWorldLocation, const FTransform& AgentTransform);
	FLearningAgentsObservationObjectElement GatherCombatHistoryObservation(const FLAObservationGatherParams& GatheringParams, const ULAObservationHistoryComponent* ObservationHistoryComponent);
	FLearningAgentsObservationObjectElement GatherTranslationHistoryObservation(
		const FLAObservationGatherParams& GatheringParams, const ULAObservationHistoryComponent* ObservationHistoryComponent);
	FLearningAgentsObservationObjectElement GatherOtherCharacterDynamicObservations(ULearningAgentsObservationObject* InObservationObject,
	                                                                                int32 AgentId, const UCombatLearningSettings* Settings, const TArray<float>* RaindropsTo,
	                                                                                const FVector& AgentWorldLocation, const FTransform& AgentTransform, const FRotator& AgentRotation,
	                                                                                const FPerceivedCharacterData& ActorState, const TMap<FName, FLearningAgentsObservationObjectElement>& AdditionalObservations);
	
	
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

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsInteractor.h"
#include "Components/LAObservationHistoryComponent.h"
#include "Components/LACombatActionsComponent.h"
#include "Components/LACombatObservationComponent.h"
#include "Components/LALocomotionActionsComponent.h"
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
	struct FObservationGatherParams
	{
		ULearningAgentsObservationObject* InObservationObject = nullptr;
		const UCombatLearningSettings* Settings = nullptr;
		AActor* AgentActor = nullptr;
		FVector AgentWorldLocation = FVector::ZeroVector;
		FTransform AgentTransform = FTransform::Identity;
		int AgentId = 0;
	};
	
	using FObservationSchemaItem = FLearningAgentsObservationSchemaElement;
	using FObservationObjectItem = FLearningAgentsObservationObjectElement;
	using FObservationSchemasMap = TMap<FName, FObservationSchemaItem>;
	using FObservationObjectsMap = TMap<FName, FObservationObjectItem>;

public:
	virtual void SpecifyAgentObservation_Implementation(FObservationSchemaItem& OutObservationSchemaElement,
		ULearningAgentsObservationSchema* InObservationSchema) override;
	virtual void GatherAgentObservation_Implementation(FObservationObjectItem& OutObservationObjectElement, 
	                                                   ULearningAgentsObservationObject* InObservationObject, const int32 AgentId) override;
	virtual void SpecifyAgentAction_Implementation(FLearningAgentsActionSchemaElement& OutActionSchemaElement,
	                                               ULearningAgentsActionSchema* InActionSchema) override;
	virtual void MakeAgentActionModifier_Implementation(FLearningAgentsActionModifierElement& OutActionModifierElement, 
	                                                    ULearningAgentsActionModifier* InActionModifier, const ULearningAgentsObservationObject* InObservationObject,
	                                                    const FObservationObjectItem& InObservationObjectElement, const int32 AgentId) override;
	virtual void PerformAgentAction_Implementation(const ULearningAgentsActionObject* InActionObject,
	                                               const FLearningAgentsActionObjectElement& InActionObjectElement, const int32 AgentId) override;
	
protected:
	virtual FObservationSchemasMap SpecifySelfObservations(ULearningAgentsObservationSchema* InObservationSchema,
		const UCombatLearningSettings* Settings) const;
	virtual FObservationSchemasMap SpecifyLidarSelfObservations(ULearningAgentsObservationSchema* InObservationSchema, 
		const UCombatLearningSettings* Settings) const;
	virtual FObservationSchemaItem SpecifyOtherCharacterDynamicObservations(ULearningAgentsObservationSchema* InObservationSchema,
		const UCombatLearningSettings* Settings,
		const FObservationSchemasMap& AdditionalObservations, ELAAgentAttitude RaindropTarget) const;
	virtual FObservationSchemaItem SpecifyOtherCharacterStaticObservations(ULearningAgentsObservationSchema* InObservationSchema, 
		const UCombatLearningSettings* Settings, const FObservationSchemasMap& AdditionalObservations) const;
	virtual FObservationSchemaItem SpecifyEnemyObservation(ULearningAgentsObservationSchema* InObservationSchema, 
		const UCombatLearningSettings* Settings) const;
	virtual FObservationSchemaItem SpecifyAllyObservations(ULearningAgentsObservationSchema* InObservationSchema, 
		const UCombatLearningSettings* Settings) const;
	virtual FObservationSchemasMap SpecifyCombatStateObservation(ULearningAgentsObservationSchema* InObservationSchema) const;
	virtual FObservationSchemaItem SpecifyCombatHistoryObservation(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const;
	virtual FObservationSchemaItem SpecifyTranslationHistoryObservation(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const;
	
	virtual FObservationObjectItem GatherSelfObservations(ULearningAgentsObservationObject* InObservationObject, int AgentId, const FSelfData& SelfData);
	virtual FObservationObjectItem GatherSurroundingsObservations(ULearningAgentsObservationObject* InObservationObject,
		int32 AgentId, ULACombatObservationComponent* LAObservationComponent);
	virtual FObservationObjectItem GatherCombatStateObservation(ULearningAgentsObservationObject* InObservationObject, 
		int32 AgentId, const FCombatStateData& CombatStateData);
	virtual FObservationObjectItem GatherEnemiesObservation(ULearningAgentsObservationObject* InObservationObject, 
		ULACombatObservationComponent* CombatObservationComponent, int32 AgentId);
	virtual FObservationObjectItem GatherAlliesObservations(ULearningAgentsObservationObject* InObservationObject,
		ULACombatObservationComponent* CombatObservationComponent, int32 AgentId);
	
	virtual const UEnum* GetAttackEnum() { return StaticEnum<ELAAttackType>(); }
	virtual TMap<uint8, float> GetAttackEnumBaseProbabilities() const { return {{0, 1.f }}; }
	virtual TArray<uint8> GetMaskedAttackValues() const { return { }; }
	
private:
	FObservationSchemaItem SpecifyNamedExclusiveDiscreteObservation(ULearningAgentsObservationSchema* InObservationSchema,
		const TMap<FGameplayTag, float>& ObservationOptions, const FName& ObservationName, const FName& ObservationOptionalWrapperName) const;
	FObservationSchemaItem GetWeaponObservationSchema(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* LearningSettings) const;
	FObservationObjectItem GetWeaponObservation(ULearningAgentsObservationObject* AgentObject, int AgentId, const FWeaponData& WeaponData,
		const UCombatLearningSettings* Settings, const FVector& AgentLocation);
	FObservationSchemaItem SpecifyDynamicObservations(
		ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings,
		const FObservationSchemasMap& ExtraObservations) const;
	FObservationSchemaItem SpecifyStaticObservations(
		ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings, const FObservationSchemasMap& AdditionalObservations) const;
	FObservationObjectItem GatherOptionalNamedExclusiveObservation(ULearningAgentsObservationObject* InObservationObject,
		const FName& ObservationName, const FName& ObservationOptionalWrapperName, const FGameplayTag& OptionTag,
		const UCombatLearningSettings* Settings, const FVector& AgentWorldLocation, int AgentId);
	FObservationObjectItem GatherStaticObservations(const FObservationGatherParams& GatheringParams,
		const FCharacterDataBase& CharacterData, const FObservationObjectsMap& AdditionalObservations);
	FObservationObjectItem GatherDynamicObservations(const FObservationGatherParams& GatheringParams, const FCharacterDataBase& CharacterData,
		const FObservationObjectsMap& AdditionalObservations);
	FObservationObjectItem GatherCombatHistoryObservation(const FObservationGatherParams& GatheringParams,
		const ULAObservationHistoryComponent* ObservationHistoryComponent);
	FObservationObjectItem GatherTranslationHistoryObservation(const FObservationGatherParams& GatheringParams,
		const ULAObservationHistoryComponent* ObservationHistoryComponent);
	FObservationObjectItem GatherOtherCharacterDynamicObservations(const FObservationGatherParams& GatheringParams, const TArray<float>* RaindropsTo,
	    const FPerceivedCharacterData& ActorState, const FObservationObjectsMap& AdditionalObservations);
	FObservationObjectItem GatherOtherCharacterStaticObservations(const FObservationGatherParams& GatheringParams,
		const FPerceivedCharacterData& ActorState, const FObservationObjectsMap& AdditionalObservations);
	
	bool GetSelfStates(const ULearningAgentsObservationObject* InObservationObject, const FObservationObjectItem& InObservationObjectElement,
		ELACharacterStates& OutSelfStates) const;
	
	TSet<FName> GetMaskedActions(ELACharacterStates SelfStates) const;
	FLearningAgentsActionSchemaElement GetNamedOptionsActionSchemaElement(ULearningAgentsActionSchema* InActionSchema,
	    const TMap<FGameplayTag, float>& Options, const FName& ActionTag) const;
	void SampleLocomotionAction(const ULearningAgentsActionObject* InActionObject, int32 AgentId, AActor* AgentActor,
		ULALocomotionActionsComponent* LocomotionActionsComponent, const FLearningAgentsActionObjectElement& RootActionObjectElement);
	void SampleNonBlockingLocomotionActions(const ULearningAgentsActionObject* InActionObject, 
	                                        const TMap<FName, FLearningAgentsActionObjectElement>& NonBlockingLocomotionActionObjectElements, 
	                                        ULALocomotionActionsComponent* LocomotionActionsComponent, int32 AgentId, const FVector& AgentLocation, const FTransform& AgentTransform, 
	                                        const UCombatLearningSettings* Settings);
	void SampleLocomotionAnimationAction(const ULearningAgentsActionObject* InActionObject, const FName& AnimationActionName,
										 const FLearningAgentsActionObjectElement& AnimationActionObjectElement,
										 ULALocomotionActionsComponent* LocomotionActionsComponent,
										 int32 AgentId, const FVector& AgentLocation,
										 const UCombatLearningSettings* Settings);
	
	void SampleCombatAction(const ULearningAgentsActionObject* InActionObject, int32 AgentId, const AActor* AgentActor,
							ULACombatActionsComponent* CombatActionsComponent,
							const FLearningAgentsActionObjectElement& RootActionObjectElement);
	TArray<uint8> GetBlockingLocomotionActionsMask(ELACharacterStates SelfStates) const;
	TArray<uint8> GetWeaponStateChangeMask(ELACharacterStates SelfStates) const;
	
};

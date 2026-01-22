// 
#include "LearningEntities/Interactors/LearningAgentsInteractor_Combat.h"

#include "Components/LAObservationHistoryComponent.h"
#include "Components/LearningAgentCombatActionsComponent.h"
#include "Components/LearningAgentCombatObservationComponent.h"
#include "Components/LearningAgentLocomotionActionsComponent.h"
#include "Data/LearningAgentsDataTypes.h"
#include "Data/LearningAgentsSchemaKeys.h"
#include "Data/LearningAgentsTags_Combat.h"
#include "Settings/CombatLearningSettings.h"

using namespace LAActionKeys;
using namespace LAObservationKeys;

void ULearningAgentsInteractor_Combat::SpecifyAgentObservation_Implementation(
	FLearningAgentsObservationSchemaElement& OutObservationSchemaElement,
	ULearningAgentsObservationSchema* InObservationSchema)
{
	Super::SpecifyAgentObservation_Implementation(OutObservationSchemaElement, InObservationSchema);
	TMap<FName, FLearningAgentsObservationSchemaElement> Observations;
	auto Settings = GetDefault<UCombatLearningSettings>();
	
	if (Settings->RelevantObservations.HasTag(LearningAgentsTags_Combat::Observation_Self))
	{
		auto SelfObservations = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema,
		SpecifySelfObservations(InObservationSchema, Settings), Key_Observation_Self);
		Observations.Add(Key_Observation_Self, SelfObservations);
	}
	
	if (Settings->RelevantObservations.HasTag(LearningAgentsTags_Combat::Observation_Surrounding))
	{
		auto LidarObservations = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema,
		SpecifyLidarSelfObservations(InObservationSchema, Settings), Key_Observation_Surrounding_LIDAR);
		Observations.Add(Key_Observation_Surrounding_LIDAR, LidarObservations);
	}

	if (Settings->RelevantObservations.HasTag(LearningAgentsTags_Combat::Observation_Enemy))
	{
		auto EnemiesObservations = ULearningAgentsObservations::SpecifyArrayObservation(InObservationSchema,
			SpecifyEnemyObservation(InObservationSchema, Settings),
			Settings->MaxEnemiesObservedAtOnce, 128, 4, 128, Key_Observation_Enemies);
		Observations.Add(Key_Observation_Enemies, EnemiesObservations);
	}

	if (Settings->RelevantObservations.HasTag(LearningAgentsTags_Combat::Observation_Ally))
	{
		auto AlliesObservations = ULearningAgentsObservations::SpecifyArrayObservation(InObservationSchema,
	   SpecifyAllyObservations(InObservationSchema, Settings),
			Settings->MaxAlliesObservedAtOnce, 128, 4, 128, Key_Observation_Allies);
		Observations.Add(Key_Observation_Allies, AlliesObservations);
	}

	if (Settings->RelevantObservations.HasTag(LearningAgentsTags_Combat::Observation_CombatState))
	{
		auto EnvironmentObservations = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema,
			SpecifyCombatStateObservation(InObservationSchema), Key_Observation_Combat_State);
		Observations.Add(Key_Observation_Combat_State, EnvironmentObservations);
	}
	
	OutObservationSchemaElement = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, Observations, Key_CombatObservations);
}

/*
 * Actions:
 * Move
 * Rotate
 * Set move speed
 * Jump
 *		optional
 * Attack
 * Feint
 * Dodge
 * Parry
 * Block (for shields only?)
 * Consume item
 * Gesture
 * Say phrase
 * Sheathe weapon
 * Unsheathe weapon
 */

void ULearningAgentsInteractor_Combat::SpecifyAgentAction_Implementation(
	FLearningAgentsActionSchemaElement& OutActionSchemaElement, ULearningAgentsActionSchema* InActionSchema)
{
	Super::SpecifyAgentAction_Implementation(OutActionSchemaElement, InActionSchema);
	
	auto Settings = GetDefault<UCombatLearningSettings>();
	
	TMap<uint8, float> ChangeWeaponStateChance = 
	{
		{ static_cast<uint8>(ELAWeaponStateChange::Unready), 0.05f },
		{ static_cast<uint8>(ELAWeaponStateChange::Ready), 0.95f }
	};
	
	auto ChangeWeaponStateAction = ULearningAgentsActions::SpecifyEnumAction(InActionSchema, StaticEnum<ELAWeaponStateChange>(), 
		ChangeWeaponStateChance, Key_Action_ChangeWeaponState);
	
	TMap<FName, FLearningAgentsActionSchemaElement> ExclusiveAnimationLocomotionActions = 
	{
		{ Key_Action_ChangeWeaponState, ChangeWeaponStateAction }
	};
	
	TMap<FName, float> ExclusiveAnimationLocomotionActionsProbabilities = 
	{
		{ Key_Action_ChangeWeaponState, 0.5f }
	};
	
	if (!Settings->ConsumablesProbabilities.IsEmpty())
	{
		FLearningAgentsActionSchemaElement UseConsumableItemAction = GetNamedOptionsActionSchemaElement(InActionSchema,
			Settings->ConsumablesProbabilities, Key_Action_UseConsumableItem);
		ExclusiveAnimationLocomotionActions.Emplace(Key_Action_UseConsumableItem, UseConsumableItemAction);
		ExclusiveAnimationLocomotionActionsProbabilities.Emplace(Key_Action_UseConsumableItem, 0.15f);
	}
	
	if (!Settings->Gestures.IsEmpty())
	{
		FLearningAgentsActionSchemaElement GestureAction = GetNamedOptionsActionSchemaElement(InActionSchema, Settings->Gestures,
			Key_Action_Gesture);
		ExclusiveAnimationLocomotionActions.Emplace(Key_Action_Gesture, GestureAction);
		ExclusiveAnimationLocomotionActionsProbabilities.Emplace(Key_Action_Gesture, 0.2f);
	}
	
	auto AnimationNonBlockingAction = ULearningAgentsActions::SpecifyExclusiveUnionAction(InActionSchema, ExclusiveAnimationLocomotionActions,
		ExclusiveAnimationLocomotionActionsProbabilities, Key_Action_Locomotion_NonBlocking_Animation);
	
	auto SetSpeedAction = ULearningAgentsActions::SpecifyFloatAction(InActionSchema, Settings->MaxSpeed, Key_Action_Locomotion_SetSpeed);
	auto MoveAction = ULearningAgentsActions::SpecifyDirectionAction(InActionSchema, Key_Action_Locomotion_Move);
	auto RotateAction = ULearningAgentsActions::SpecifyRotationAction(InActionSchema, Settings->RotationScale, Key_Action_Locomotion_Rotate);
	
	// auto JumpActionOptional = ULearningAgentsActions::SpecifyOptionalAction(InActionSchema, JumpAction, 0.2f, Key_Action_Jump_Optional);
	
	TMap<uint8, float> LocomotionActionsProbabilities = 
	{
		{ static_cast<uint8>(ELALocomotionAction::Jump), 0.25f, },
		{ static_cast<uint8>(ELALocomotionAction::Mantle), 0.25f, },
	};
	
	auto BlockingLocomotionAction = ULearningAgentsActions::SpecifyEnumAction(InActionSchema, StaticEnum<ELALocomotionAction>(), 
		LocomotionActionsProbabilities, Key_Action_Locomotion_Blocking);
	
	auto DodgeAction = ULearningAgentsActions::SpecifyDirectionAction(InActionSchema, Key_Action_Combat_Dodge);
	// auto DodgeActionOptional = ULearningAgentsActions::SpecifyOptionalAction(InActionSchema, DodgeAction, 0.1f, Key_Action_Dodge_Optional);
	
	auto ParryAction = ULearningAgentsActions::SpecifyFloatAction(InActionSchema, 360.f, Key_Action_Combat_Block);
	// auto ParryActionOptional = ULearningAgentsActions::SpecifyOptionalAction(InActionSchema, ParryAction, 0.2f, Key_Action_Parry_Optional);
	
	auto AttackAction = ULearningAgentsActions::SpecifyEnumAction(InActionSchema, GetAttackEnum(), GetAttackEnumBaseProbabilities(),
		Key_Action_Combat_Attack);
	
	TMap<FName, FLearningAgentsActionSchemaElement> NonBlockingLocomotionActions = 
	{
		{ Key_Action_Locomotion_SetSpeed, SetSpeedAction },
		{ Key_Action_Locomotion_Move, MoveAction },
		{ Key_Action_Locomotion_Rotate, RotateAction },
		{ Key_Action_Locomotion_NonBlocking_Animation, AnimationNonBlockingAction }
	};
	
	// i'm not sure if probabilities should sum up to exactly 1 or not 
	TMap<FName, float> NonBlockingLocomotionActionsProbabilities = 
	{
		{ Key_Action_Locomotion_SetSpeed, 0.2f },
		{ Key_Action_Locomotion_Move, 0.7f, },
		{ Key_Action_Locomotion_Rotate, 0.4f },
		{ Key_Action_Locomotion_NonBlocking_Animation, 0.2f }
	};
	
	if (!Settings->Phrases.IsEmpty())
	{
		FLearningAgentsActionSchemaElement SayPhraseAction = GetNamedOptionsActionSchemaElement(InActionSchema, Settings->Phrases,
			Key_Action_SayPhrase);
		NonBlockingLocomotionActions.Emplace(Key_Action_SayPhrase, SayPhraseAction);
		NonBlockingLocomotionActionsProbabilities.Emplace(Key_Action_SayPhrase, 0.2f);
	}
	
	auto NonBlockingLocomotionActionsUnion = ULearningAgentsActions::SpecifyInclusiveUnionAction(InActionSchema, 
		NonBlockingLocomotionActions, NonBlockingLocomotionActionsProbabilities,
		Key_Action_Locomotion_NonBlocking);
	
	TMap<FName, FLearningAgentsActionSchemaElement> ExclusiveLocomotionActionsMap = 
	{
		{ Key_Action_Locomotion_Blocking, BlockingLocomotionAction },
		{ Key_Action_Locomotion_NonBlocking, NonBlockingLocomotionActionsUnion },
	};
	
	TMap<FName, float> ExclusiveLocomotionActionsProbabilities = 
	{
		{ Key_Action_Locomotion_Blocking, 0.15, },
		{ Key_Action_Locomotion_NonBlocking, 0.85f },
	};
	
	auto ExclusiveLocomotionActionsUnion = ULearningAgentsActions::SpecifyExclusiveUnionAction(InActionSchema,
		ExclusiveLocomotionActionsMap, ExclusiveLocomotionActionsProbabilities, Key_Action_Locomotion);
	
	TMap<FName, FLearningAgentsActionSchemaElement> CombatActions = 
	{
		{ Key_Action_Combat_Attack, AttackAction },
		{ Key_Action_Combat_Block, ParryAction },
		{ Key_Action_Combat_Dodge, DodgeAction },
	};
	
	TMap<FName, float> CombatActionsProbabilities = 
	{
		{ Key_Action_Combat_Attack, 0.6f },
		{ Key_Action_Combat_Block, 0.3f },
		{ Key_Action_Combat_Dodge, 0.2f },
	};
	
	auto CombatActionsExclusiveUnion = ULearningAgentsActions::SpecifyExclusiveUnionAction(InActionSchema, CombatActions, CombatActionsProbabilities,
		Key_Action_Combat);
	
	auto NullAction = ULearningAgentsActions::SpecifyNullAction(InActionSchema, Key_Action_Null);
	TMap<FName, FLearningAgentsActionSchemaElement> AllActions = 
	{
		{ Key_Action_Locomotion, ExclusiveLocomotionActionsUnion },
		{ Key_Action_Combat, CombatActionsExclusiveUnion },
		{ Key_Action_Null, NullAction }
	};
	
	TMap<FName, float> AllActionsProbabilities = 
	{
		{ Key_Action_Locomotion, 0.6f },
		{ Key_Action_Combat, 0.3f },
		{ Key_Action_Null, 0.1f }
	};
	
	OutActionSchemaElement = ULearningAgentsActions::SpecifyExclusiveUnionAction(InActionSchema, AllActions, AllActionsProbabilities, Key_Action_All);
}

FLearningAgentsActionSchemaElement ULearningAgentsInteractor_Combat::GetNamedOptionsActionSchemaElement(ULearningAgentsActionSchema* InActionSchema,
	const TMap<FGameplayTag, float>& Options, const FName& ActionTag) const
{
	TArray<FName> OptionNames;
	TMap<FName, float> OptionProbabilities;
	OptionNames.Reserve(Options.Num());
	OptionProbabilities.Reserve(Options.Num());
		
	for (const auto& Option : Options)
	{
		auto OptionName = Option.Key.GetTagName();
			
		OptionNames.Emplace(OptionName);
		OptionProbabilities.Emplace(OptionName, Option.Value);
	}
		
	return ULearningAgentsActions::SpecifyNamedExclusiveDiscreteAction(InActionSchema, OptionNames, OptionProbabilities, ActionTag);
}

// bruh this boilerplate is killing me
// i hope epics do some more convenient way to edit observations and action schemas
void ULearningAgentsInteractor_Combat::MakeAgentActionModifier_Implementation(
	FLearningAgentsActionModifierElement& OutActionModifierElement, ULearningAgentsActionModifier* InActionModifier,
	const ULearningAgentsObservationObject* InObservationObject,
	const FLearningAgentsObservationObjectElement& InObservationObjectElement, const int32 AgentId)
{
	Super::MakeAgentActionModifier_Implementation(OutActionModifierElement, InActionModifier, InObservationObject,
	                                              InObservationObjectElement, AgentId);

	ELACharacterStates SelfStates = ELACharacterStates::None;
	bool bGotSelfStates = GetSelfStates(InObservationObject, InObservationObjectElement, SelfStates);
	if (!bGotSelfStates)
		return;

	auto Settings = GetDefault<UCombatLearningSettings>();
	TSet<FName> AllMaskedActions = GetMaskedActions(SelfStates);
	
	// TODO 26.11.2025: think about the following questions
	// 1. I'm not sure, but maybe I need to use separate set of tags for action modifiers instead of reusing tags of defined actions
	// 2. Maybe instead of having separate masked action arrays for each element of action schema hierarchy just use single AllMaskedActions from GetMaskedActions ? 
	//		I need to understand the source code better to answer this
	TArray<FName> MaskedTopLevelActions;
	if (AllMaskedActions.Contains(Key_Action_Locomotion))
		MaskedTopLevelActions.Emplace(Key_Action_Locomotion);
	
	if (AllMaskedActions.Contains(Key_Action_Combat))
		MaskedTopLevelActions.Emplace(Key_Action_Combat);
	
	// =========================== combat actions modifiers
	
	// i don't really know if I should provide any modifiers for attack, parry and dodge actions. 
	// it's like there isn't anything meaningful I could specify for them
	auto AttackActionModifier = ULearningAgentsActions::MakeEnumActionModifier(InActionModifier, GetAttackEnum(),
		GetMaskedAttackValues(), Key_Action_Combat_Attack);
	auto ParryActionModifier = ULearningAgentsActions::MakeFloatActionModifier(InActionModifier,
		0.f, AllMaskedActions.Contains(Key_Action_Combat_Block), Key_Action_Combat_Block);
	auto DodgeActionModifier = ULearningAgentsActions::MakeDirectionActionModifier(InActionModifier, 
		FVector::ZeroVector, false, false, true, FTransform::Identity,
		Key_Action_Combat_Dodge);
	
	TMap<FName, FLearningAgentsActionModifierElement> CombatActionModifiersMap = 
	{
		{ Key_Action_Combat_Attack, AttackActionModifier },
		{ Key_Action_Combat_Block, ParryActionModifier },
		{ Key_Action_Combat_Dodge, DodgeActionModifier },
	};
	
	TSet<FName> AllCombatActions = { Key_Action_Combat_Attack, Key_Action_Combat_Block, Key_Action_Combat_Dodge };
	TArray<FName> MaskedCombatActions = AllMaskedActions.Intersect(AllCombatActions).Array();
	auto CombatActionsModifier = ULearningAgentsActions::MakeExclusiveUnionActionModifier(InActionModifier, CombatActionModifiersMap, 
		MaskedCombatActions);

	// =================================== non blocking locomotion actions modifiers
	
	const bool bHasGestureAction = !Settings->Gestures.IsEmpty();
	const bool bHasSayPhraseAction = !Settings->Phrases.IsEmpty();
	const bool bHasUseConsumableItemAction = !Settings->ConsumablesProbabilities.IsEmpty();
	
	TSet<FName> AllNonBlockingLocomotionActions = 
	{
		Key_Action_Locomotion_SetSpeed, Key_Action_Locomotion_Move,
		Key_Action_Locomotion_Rotate, Key_Action_Locomotion_NonBlocking_Animation
	};
	
	if (bHasSayPhraseAction)
		AllNonBlockingLocomotionActions.Emplace(Key_Action_SayPhrase);

	TSet<FName> AllLocomotionNonBlockingAnimationActions = { Key_Action_ChangeWeaponState };
	
	if (bHasGestureAction)
		AllLocomotionNonBlockingAnimationActions.Emplace(Key_Action_Gesture);
	if (bHasUseConsumableItemAction)
		AllLocomotionNonBlockingAnimationActions.Emplace(Key_Action_UseConsumableItem);
	
	TArray<FName> MaskedNonBlockingLocomotionActions = AllMaskedActions.Intersect(AllNonBlockingLocomotionActions).Array();
	TArray<FName> MaskedAnimationActions = AllMaskedActions.Intersect(AllLocomotionNonBlockingAnimationActions).Array();
	
	bool bSetSpeedActionMasked = MaskedNonBlockingLocomotionActions.Contains(Key_Action_Locomotion_SetSpeed);
	auto SetSpeedActionModifier = ULearningAgentsActions::MakeFloatActionModifier(InActionModifier,
		0.f, bSetSpeedActionMasked, Key_Action_Locomotion_SetSpeed);
	auto MoveActionModifier = ULearningAgentsActions::MakeDirectionActionModifier(InActionModifier, 
		FVector::ZeroVector, false, false, false, FTransform::Identity,
		Key_Action_Locomotion_Move);
	// well, fuck. There's no function to get a modifier for a rotate action.
	// I guess we have to find out what's going to happen when specifying a modifier of a wrong type
	auto RotateActionModifier = ULearningAgentsActions::MakeDirectionActionModifier(InActionModifier, 
		FVector::ZeroVector, false, false, false, FTransform::Identity, 
		Key_Action_Locomotion_Rotate);
	
	auto ChangeWeaponStateActionModifier = ULearningAgentsActions::MakeEnumActionModifier(InActionModifier, StaticEnum<ELAWeaponStateChange>(), 
	GetWeaponStateChangeMask(SelfStates), Key_Action_ChangeWeaponState);
	
	TMap<FName, FLearningAgentsActionModifierElement> AnimationActionModifiersMap = 
	{
		{ Key_Action_ChangeWeaponState, ChangeWeaponStateActionModifier }
	};
	
	if (bHasGestureAction)
	{
		auto GestureActionModifier = ULearningAgentsActions::MakeNamedExclusiveDiscreteActionModifier(InActionModifier, {}, Key_Action_Gesture);
		AnimationActionModifiersMap.Emplace(Key_Action_Gesture, GestureActionModifier);
	}
	
	if (bHasUseConsumableItemAction)
	{
		auto UseConsumableItemModifier = ULearningAgentsActions::MakeNamedExclusiveDiscreteActionModifier(InActionModifier, {}, Key_Action_UseConsumableItem);
		AnimationActionModifiersMap.Emplace(Key_Action_UseConsumableItem, UseConsumableItemModifier);
	}

	auto AnimationActionsUnionModifier = ULearningAgentsActions::MakeExclusiveUnionActionModifier(InActionModifier, 
		AnimationActionModifiersMap, MaskedAnimationActions, Key_Action_Locomotion_NonBlocking_Animation);

	
	TMap<FName, FLearningAgentsActionModifierElement> NonBlockingLocomotionActionModifiersMap = 
	{
		{ Key_Action_Locomotion_SetSpeed, SetSpeedActionModifier },
		{ Key_Action_Locomotion_Move, MoveActionModifier },
		{ Key_Action_Locomotion_Rotate, RotateActionModifier },
		{ Key_Action_Locomotion_NonBlocking_Animation, AnimationActionsUnionModifier }
	};
	
	if (bHasSayPhraseAction)
	{
		auto SayPhraseActionModifier = ULearningAgentsActions::MakeNamedExclusiveDiscreteActionModifier(InActionModifier, {}, Key_Action_SayPhrase);
		NonBlockingLocomotionActionModifiersMap.Emplace(Key_Action_SayPhrase, SayPhraseActionModifier);
	}
	
	auto NonBlockingLocomotionUnionActionModifier = ULearningAgentsActions::MakeInclusiveUnionActionModifier(InActionModifier, 
		NonBlockingLocomotionActionModifiersMap, MaskedNonBlockingLocomotionActions, Key_Action_Locomotion_NonBlocking);
	
	// ===================================
	
	auto BlockingLocomotionActionsModifier = ULearningAgentsActions::MakeEnumActionModifier(InActionModifier, 
		StaticEnum<ELALocomotionAction>(), GetBlockingLocomotionActionsMask(SelfStates),
		Key_Action_Locomotion_Blocking);
	
	TMap<FName, FLearningAgentsActionModifierElement> LocomotionActionModifiersMap = 
	{
		{ Key_Action_Locomotion_Blocking, BlockingLocomotionActionsModifier },
		{ Key_Action_Locomotion_NonBlocking, NonBlockingLocomotionUnionActionModifier },
	};
	
	TSet<FName> AllLocomotionActions = { Key_Action_Locomotion_Blocking, Key_Action_Locomotion_NonBlocking };
	TArray<FName> MaskedLocomotionActions = AllMaskedActions.Intersect(AllLocomotionActions).Array();
	
	auto LocomotionActionsModifier = ULearningAgentsActions::MakeExclusiveUnionActionModifier(InActionModifier, LocomotionActionModifiersMap,
		MaskedLocomotionActions);
	
	auto RootLevelNullActionModifier = ULearningAgentsActions::MakeNullActionModifier(InActionModifier, Key_Action_Null);
	TMap<FName, FLearningAgentsActionModifierElement> RootActionModifiers = 
	{
		{ Key_Action_Locomotion, LocomotionActionsModifier },
		{ Key_Action_Combat, CombatActionsModifier },
		{ Key_Action_Null, RootLevelNullActionModifier }
	};
	
	OutActionModifierElement = ULearningAgentsActions::MakeExclusiveUnionActionModifier(InActionModifier, RootActionModifiers, 
		MaskedTopLevelActions, Key_Action_All);
}

void ULearningAgentsInteractor_Combat::GatherAgentObservation_Implementation(
	FLearningAgentsObservationObjectElement& OutObservationObjectElement,
	ULearningAgentsObservationObject* InObservationObject, const int32 AgentId)
{
	Super::GatherAgentObservation_Implementation(OutObservationObjectElement, InObservationObject, AgentId);
	auto Agent = Cast<AActor>(GetAgent(AgentId));
	auto CombatObservationComponent = Agent->FindComponentByClass<ULearningAgentCombatObservationComponent>();
	if (!ensure(CombatObservationComponent))
		return;

	TMap<FName, FLearningAgentsObservationObjectElement> Observations;
	
	auto Settings = GetDefault<UCombatLearningSettings>();
	if (Settings->RelevantObservations.HasTag(LearningAgentsTags_Combat::Observation_Self))
	{
		auto SelfData = CombatObservationComponent->GetSelfData();
		auto SelfObservation = GatherSelfObservations(InObservationObject, AgentId, SelfData);
		Observations.Add(Key_Observation_Self, SelfObservation);
	}
	
	if (Settings->RelevantObservations.HasTag(LearningAgentsTags_Combat::Observation_Surrounding))
	{
		auto SurroundingsObservation = GatherSurroundingsObservations(InObservationObject, AgentId, CombatObservationComponent);
		Observations.Add(Key_Observation_Surrounding_LIDAR, SurroundingsObservation);
	}
	
	if (Settings->RelevantObservations.HasTag(LearningAgentsTags_Combat::Observation_CombatState))
	{
		auto CombatStateData = CombatObservationComponent->GetCombatStateData();
		auto CombatStateObservation = GatherCombatStateObservation(InObservationObject, AgentId, CombatStateData);
		Observations.Add(Key_Observation_Combat_State, CombatStateObservation);
	}
	
	if (Settings->RelevantObservations.HasTag(LearningAgentsTags_Combat::Observation_Enemy))
	{
		auto EnemiesObservations = GatherEnemiesObservation(InObservationObject, CombatObservationComponent, AgentId);
		Observations.Add(Key_Observation_Enemies, EnemiesObservations);
	}

	if (Settings->RelevantObservations.HasTag(LearningAgentsTags_Combat::Observation_Ally))
	{
		auto AlliesObservation = GatherAlliesObservations(InObservationObject, CombatObservationComponent, AgentId);
		Observations.Add(Key_Observation_Allies, AlliesObservation);
	}

	OutObservationObjectElement = ULearningAgentsObservations::MakeStructObservation(InObservationObject, Observations, Key_CombatObservations);
}

void ULearningAgentsInteractor_Combat::PerformAgentAction_Implementation(
	const ULearningAgentsActionObject* InActionObject, const FLearningAgentsActionObjectElement& InActionObjectElement,
	const int32 AgentId)
{
	Super::PerformAgentAction_Implementation(InActionObject, InActionObjectElement, AgentId);
	
	auto AgentActor = Cast<AActor>(GetAgent(AgentId));
	
	auto CombatActionsComponent = AgentActor->FindComponentByClass<ULearningAgentCombatActionsComponent>();
	auto LocomotionActionsComponent = AgentActor->FindComponentByClass<ULearningAgentLocomotionActionsComponent>();
	
	CombatActionsComponent->Reset();
	LocomotionActionsComponent->Reset();
	
	FName RootActionName;
	FLearningAgentsActionObjectElement RootActionObjectElement;
	bool bGotRootAction = ULearningAgentsActions::GetExclusiveUnionAction(RootActionName, RootActionObjectElement, 
		InActionObject, InActionObjectElement, Key_Action_All);
	if (!ensure(bGotRootAction))
		return;
	
	if (RootActionName == Key_Action_Combat)
	{
		SampleCombatAction(InActionObject, AgentId, AgentActor, CombatActionsComponent, RootActionObjectElement);
	}	
	else if (RootActionName == Key_Action_Locomotion)
	{
		SampleLocomotionAction(InActionObject, AgentId, AgentActor, LocomotionActionsComponent, RootActionObjectElement);			
	}
	else
	{
		ensure(RootActionName == Key_Action_Null);
	}
}

FLearningAgentsObservationSchemaElement ULearningAgentsInteractor_Combat::GetWeaponObservationSchema(ULearningAgentsObservationSchema* InObservationSchema,
	const UCombatLearningSettings* LearningSettings) const
{
	// TODO 11.11.2025 (aki) I'm not sure if it should be the size of float or whatever other wrapper struct that is used for float
	// deepseek suggests it's just the size of the float so 32 bits
	// chat gpt says it's "number of floats in the encoded vector"
	//		or "It is the size of the latent feature vector that LA will generate for each branch (A and B) before merging them."
	//		so in theory it could be just "1", but encoding is a more complex concept than just "how much memory is needed to fit this data"
	//		so chat gipiti suggests values like 16 or 32 for floats 
	// i looked at source code and i'm more inclined to trust chat gipiti here
	int FloatObservationEncodingSize = 16;
	
	auto ActiveWeaponTypeObservation = ULearningAgentsObservations::SpecifyEnumObservation(InObservationSchema,
		StaticEnum<ELAActiveWeaponType>(), Key_Observation_ActiveWeapon_Type);
	auto WeaponMasteryObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema,
		1.f, Key_Observation_ActiveWeapon_Mastery);
	auto MeleeAttackRangeObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema,
		LearningSettings->MaxMeleeAttackRange, Key_Observation_ActiveWeapon_Range_Melee);
	auto RangedAttackRangeObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema,
		LearningSettings->MaxRangedAttackRange, Key_Observation_ActiveWeapon_Range_Ranged);
	auto AttackRangeObservation = ULearningAgentsObservations::SpecifyEitherObservation(InObservationSchema, MeleeAttackRangeObservation,
		RangedAttackRangeObservation, FloatObservationEncodingSize, Key_Observation_ActiveWeapon_Range);
	auto PowerRateObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema,
		LearningSettings->MaxPowerRate, Key_Observation_ActiveWeapon_PowerRate);
	
	TMap<FName, FLearningAgentsObservationSchemaElement> ActiveWeaponObservations =
	{
		{ Key_Observation_ActiveWeapon_Type, ActiveWeaponTypeObservation },
		{ Key_Observation_ActiveWeapon_Mastery, WeaponMasteryObservation },
		{ Key_Observation_ActiveWeapon_Range, AttackRangeObservation},
		{ Key_Observation_ActiveWeapon_PowerRate, PowerRateObservation }
	};
	
	return ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, ActiveWeaponObservations, Key_Observation_ActiveWeapon);
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GetWeaponObservation(
	ULearningAgentsObservationObject* AgentObject, int AgentId,
	const FWeaponData& WeaponData, const UCombatLearningSettings* Settings, const FVector& AgentLocation)
{
	auto ActiveWeaponTypeObservation = ULearningAgentsObservations::MakeEnumObservation(AgentObject, StaticEnum<ELAActiveWeaponType>(),
		static_cast<uint8>(WeaponData.WeaponType), Key_Observation_ActiveWeapon_Type,
		Settings->bVisLogEnabled, this, AgentId, AgentLocation);
	
	auto WeaponMasteryObservation = ULearningAgentsObservations::MakeFloatObservation(AgentObject, WeaponData.NormalizedWeaponMastery,
		Key_Observation_ActiveWeapon_Mastery,
		Settings->bVisLogEnabled, this, AgentId, AgentLocation);

	FLearningAgentsObservationObjectElement AttackRangeObservation;
	if (WeaponData.WeaponType == ELAActiveWeaponType::Ranged)
	{
		auto RangedAttackRangeObservation = ULearningAgentsObservations::MakeFloatObservation(AgentObject, WeaponData.AttackRange,
			Key_Observation_ActiveWeapon_Range_Ranged,
			Settings->bVisLogEnabled, this, AgentId, AgentLocation);
		AttackRangeObservation = ULearningAgentsObservations::MakeEitherObservation(AgentObject,
			RangedAttackRangeObservation, ELearningAgentsEitherObservation::B, Key_Observation_ActiveWeapon_Range);
	}
	else
	{
		auto MeleeAttackRangeObservation = ULearningAgentsObservations::MakeFloatObservation(AgentObject, WeaponData.AttackRange, Key_Observation_ActiveWeapon_Range_Melee,
			Settings->bVisLogEnabled, this, AgentId, AgentLocation);
		AttackRangeObservation = ULearningAgentsObservations::MakeEitherObservation(AgentObject,
			MeleeAttackRangeObservation, ELearningAgentsEitherObservation::A, Key_Observation_ActiveWeapon_Range);
	}
	
	auto PowerRate = ULearningAgentsObservations::MakeFloatObservation(AgentObject, WeaponData.PowerRate, Key_Observation_ActiveWeapon_PowerRate);
	
	TMap<FName, FLearningAgentsObservationObjectElement> ActiveWeaponObservations =
	{
		{ Key_Observation_ActiveWeapon_Type, ActiveWeaponTypeObservation },
		{ Key_Observation_ActiveWeapon_Mastery, WeaponMasteryObservation },
		{ Key_Observation_ActiveWeapon_Range, AttackRangeObservation},
		{ Key_Observation_ActiveWeapon_PowerRate, PowerRate }
	};
	
	return ULearningAgentsObservations::MakeStructObservation(AgentObject, ActiveWeaponObservations, Key_Observation_ActiveWeapon);
}

FLearningAgentsObservationSchemaElement ULearningAgentsInteractor_Combat::SpecifyDynamicObservations(
	ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings,
	const TMap<FName, FLearningAgentsObservationSchemaElement>& ExtraObservations) const
{
	auto HealthObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.f, Key_Observation_Dynamic_Health);
	auto AccumulatedDamageObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.f, Key_Observation_Dynamic_AccumulatedDamage);
	auto VelocityObservation = ULearningAgentsObservations::SpecifyVelocityObservation(InObservationSchema, Settings->MaxSpeed, Key_Observation_Dynamic_Velocity);
	auto StatesObservation = ULearningAgentsObservations::SpecifyBitmaskObservation(InObservationSchema, StaticEnum<ELACharacterStates>(), Key_Observation_Dynamic_CombatStates);
	auto WeaponObservation = GetWeaponObservationSchema(InObservationSchema, Settings);
	TMap<FName, FLearningAgentsObservationSchemaElement> DynamicSelfObservations =
	{
		{ Key_Observation_Dynamic_Health, HealthObservation },
		{ Key_Observation_Dynamic_AccumulatedDamage, AccumulatedDamageObservation },
		{ Key_Observation_Dynamic_Velocity, VelocityObservation },
		{ Key_Observation_Dynamic_CombatStates, StatesObservation },
		{ Key_Observation_ActiveWeapon, WeaponObservation },
	};
	
	// auto GestureObservation = ULearningAgentsObservations::SpecifyEnumObservation(InObservationSchema, 
	// 	StaticEnum<ELAGesture>(), Key_Observation_Self_Dynamic_Gesture);
	if (!Settings->Gestures.IsEmpty())
	{
		auto GestureOptionalObservation = SpecifyNamedExclusiveDiscreteObservation(InObservationSchema, Settings->Gestures, Key_Observation_Dynamic_Gesture, Key_Observation_Dynamic_Gesture_Optional);
		DynamicSelfObservations.Add(Key_Observation_Dynamic_Gesture_Optional, GestureOptionalObservation);
	}
	
	if (!Settings->Phrases.IsEmpty())
	{
		auto PhrasesOptionalObservation = SpecifyNamedExclusiveDiscreteObservation(InObservationSchema, Settings->Phrases, Key_Observation_Dynamic_Phrase, Key_Observation_Dynamic_Phrase_Optional);
		DynamicSelfObservations.Add(Key_Observation_Dynamic_Phrase_Optional, PhrasesOptionalObservation);
	}
	
	DynamicSelfObservations.Append(ExtraObservations);
	return ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, DynamicSelfObservations, Key_Observation_Dynamic);
}

FLearningAgentsObservationSchemaElement ULearningAgentsInteractor_Combat::SpecifyStaticObservations(ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings, 
	const TMap<FName, FLearningAgentsObservationSchemaElement>& AdditionalObservations) const
{
	auto LevelObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, Settings->MaxLevel, Key_Observation_Static_Level);
	auto ArmorRateObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, Settings->MaxArmorRate, Key_Observation_Static_ArmorRate);
	
	TMap<FName, FLearningAgentsObservationSchemaElement> StaticObservations =
	{
		{ Key_Observation_Static_Level, LevelObservation },
		{ Key_Observation_Static_ArmorRate, ArmorRateObservation },
	};
	
	StaticObservations.Append(AdditionalObservations);
	return ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, StaticObservations, Key_Observation_Static);
}

TMap<FName, FLearningAgentsObservationSchemaElement> ULearningAgentsInteractor_Combat::SpecifySelfObservations(
	ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const
{
	// TODO (aki, 11.11.2025): use RelevantObservations from settings
	
	auto StaminaObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.f, Key_Observation_Dynamic_Stamina);
	TMap<FName, FLearningAgentsObservationSchemaElement> SelfSpecificDynamicObservations =
	{
		{ Key_Observation_Dynamic_Stamina, StaminaObservation }
	};
	
	auto SurvivalDesireObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.f, Key_Observation_Static_SurvivalDesire);
	TMap<FName, FLearningAgentsObservationSchemaElement> SelfSpecificStaticObservations =
	{
		{ Key_Observation_Static_SurvivalDesire, SurvivalDesireObservation }
	};
		
	FLearningAgentsObservationSchemaElement DynamicSelfObservationsStruct = SpecifyDynamicObservations(InObservationSchema, Settings, SelfSpecificDynamicObservations);
	FLearningAgentsObservationSchemaElement StaticSelfObservationsStruct = SpecifyStaticObservations(InObservationSchema, Settings, SelfSpecificStaticObservations);
	FLearningAgentsObservationSchemaElement CombatEventHistoryObservation = SpecifyCombatHistoryObservation(InObservationSchema, Settings);
	FLearningAgentsObservationSchemaElement TranslationHistoryObservation = SpecifyTranslationHistoryObservation(InObservationSchema, Settings);
	
	TMap<FName, FLearningAgentsObservationSchemaElement> Result = 
	{
		{ Key_Observation_Static, StaticSelfObservationsStruct },
		{ Key_Observation_Dynamic, DynamicSelfObservationsStruct },
		{ Key_Observation_CombatHistory, CombatEventHistoryObservation },
		{ Key_Observation_TranslationHistory, TranslationHistoryObservation }
	};
	
	return Result;
}

TMap<FName, FLearningAgentsObservationSchemaElement> ULearningAgentsInteractor_Combat::SpecifyLidarSelfObservations(
	ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const
{
	// welp, until there's no conv3d, LAs will observe convolved "heightmaps" in different plains 
	
	auto CeilingObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, Settings->MaxCeilingHeight,
		Key_Observation_Surrounding_LIDAR_Ceiling);
	
	auto RaindropDownwardObservation = ULearningAgentsObservations::SpecifyLidarObservation(InObservationSchema,
		Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Item);
	int RaindropDownwardResolution = Settings->DownwardRaindropsParams.GetResolution();
	auto RaindropDownwardObservations = ULearningAgentsObservations::SpecifyStaticArrayObservation(InObservationSchema,
		RaindropDownwardObservation, RaindropDownwardResolution * RaindropDownwardResolution, Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Array);
	auto RaindropDownwardObservationsConvolved = ULearningAgentsObservations::SpecifyConv2dObservation(InObservationSchema,
		RaindropDownwardObservations, Settings->LidarRaindropDownwardConv2dParams, Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Convolved);
	
	auto RaindropForwardObservation = ULearningAgentsObservations::SpecifyLidarObservation(InObservationSchema,
		Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Item);
	int RaindropForwardResolution = Settings->ForwardRaindropsParams.GetResolution();
	auto RaindropForwardObservations = ULearningAgentsObservations::SpecifyStaticArrayObservation(InObservationSchema,
		RaindropForwardObservation, RaindropForwardResolution * RaindropForwardResolution, Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Array);
	auto RaindropForwardObservationsConvolved = ULearningAgentsObservations::SpecifyConv2dObservation(InObservationSchema,
		RaindropForwardObservations, Settings->LidarRaindropForwardConv2dParams, Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Convolved);
	
	auto RaindropBackwardObservation = ULearningAgentsObservations::SpecifyLidarObservation(InObservationSchema,
		Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Item);
	int RaindropBackwardResolution = Settings->BackwardRaindropsParams.GetResolution();
	auto RaindropBackwardObservations = ULearningAgentsObservations::SpecifyStaticArrayObservation(InObservationSchema,
		RaindropBackwardObservation, RaindropBackwardResolution * RaindropBackwardResolution, Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Array);
	auto RaindropBackwardObservationsConvolved = ULearningAgentsObservations::SpecifyConv2dObservation(InObservationSchema,
		RaindropBackwardObservations, Settings->LidarRaindropBackwardConv2dParams, Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Convolved);

	// TODO primary target observation. perhaps with lesser density
	
	TMap<FName, FLearningAgentsObservationSchemaElement> Result = {
		{Key_Observation_Surrounding_LIDAR_Ceiling, CeilingObservation},
		{Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Convolved, RaindropDownwardObservationsConvolved},
		{Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Convolved, RaindropForwardObservationsConvolved},
		{Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Convolved, RaindropBackwardObservationsConvolved},
	};
	
	return Result;
}

FLearningAgentsObservationSchemaElement ULearningAgentsInteractor_Combat::SpecifyOtherCharacterDynamicObservations(ULearningAgentsObservationSchema* InObservationSchema,
	const UCombatLearningSettings* Settings, const TMap<FName, FLearningAgentsObservationSchemaElement>& AdditionalObservations, ELAAgentAttitude RaindropTarget) const
{
	auto EnemyAliveObservation = ULearningAgentsObservations::SpecifyBoolObservation(InObservationSchema, Key_Observation_Actor_Dynamic_IsAlive);
	auto EnemyLocationObservation = ULearningAgentsObservations::SpecifyLocationObservation(InObservationSchema, Settings->EnemyDistanceScale, Key_Observation_Actor_Dynamic_Location);
	auto EnemyOrientationObservation = ULearningAgentsObservations::SpecifyRotationObservation(InObservationSchema, Key_Observation_Actor_Dynamic_Orientation);
	auto AgentCanSeeEnemyObservation = ULearningAgentsObservations::SpecifyBoolObservation(InObservationSchema, Key_Observation_Actor_Dynamic_AgentCanSeeActor);
	
	auto RaindropToTargetLidarObservation = ULearningAgentsObservations::SpecifyLidarObservation(InObservationSchema, 
		Key_Observation_Actor_Dynamic_RaindropTo_Lidar);
	const int RaindropToTargetResolution = Settings->GetRaindropToTargetResolution(RaindropTarget);
	auto RaindropToTargetArrayObservation = ULearningAgentsObservations::SpecifyStaticArrayObservation(InObservationSchema, RaindropToTargetLidarObservation,
		RaindropToTargetResolution, Key_Observation_Actor_Dynamic_RaindropTo_Array);
	auto Conv2dRaindropObservation = ULearningAgentsObservations::SpecifyConv2dObservation(InObservationSchema, RaindropToTargetArrayObservation,
		Settings->LidarRaindropToTargetConv2dParams[RaindropTarget], Key_Observation_Actor_Dynamic_RaindropTo_Convolved);
	int ConvolvedRaindropOptionalEncodingSize = Settings->LidarRaindropToTargetConv2dParams[RaindropTarget].InChannels 
		* Settings->LidarRaindropToTargetConv2dParams[RaindropTarget].InputHeight 
		* Settings->LidarRaindropToTargetConv2dParams[RaindropTarget].InputHeight;
	ConvolvedRaindropOptionalEncodingSize = FMath::Clamp(FMath::RoundUpToPowerOfTwo(ConvolvedRaindropOptionalEncodingSize), 128, 512);
	auto Conv2dRaindropObservationOptional = ULearningAgentsObservations::SpecifyOptionalObservation(InObservationSchema, Conv2dRaindropObservation, 
		ConvolvedRaindropOptionalEncodingSize, Key_Observation_Actor_Dynamic_RaindropTo_Convolved_Optional);
	
	TMap<FName, FLearningAgentsObservationSchemaElement> OtherCharacterDynamicObservations =
	{
		{ Key_Observation_Actor_Dynamic_IsAlive, EnemyAliveObservation},
		{ Key_Observation_Actor_Dynamic_Location, EnemyLocationObservation },
		{ Key_Observation_Actor_Dynamic_Orientation, EnemyOrientationObservation },
		{ Key_Observation_Actor_Dynamic_AgentCanSeeActor, AgentCanSeeEnemyObservation },
		{ Key_Observation_Actor_Dynamic_RaindropTo_Convolved_Optional, Conv2dRaindropObservationOptional }
	};
	
	OtherCharacterDynamicObservations.Append(AdditionalObservations);
	return SpecifyDynamicObservations(InObservationSchema, Settings, OtherCharacterDynamicObservations);
}

FLearningAgentsObservationSchemaElement ULearningAgentsInteractor_Combat::SpecifyEnemyObservation(ULearningAgentsObservationSchema* InObservationSchema,
                                                                                                    const UCombatLearningSettings* Settings) const
{
	auto KillDesireObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.f, Key_Observation_Actor_Dynamic_KillDesire);
	auto AgentCanBeSeenByEnemyObservation = ULearningAgentsObservations::SpecifyBoolObservation(InObservationSchema, Key_Observation_Actor_Dynamic_ActorCanSeeAgent);
	TMap<FName, FLearningAgentsObservationSchemaElement> AdditionalEnemyObservations = 
	{
		{ Key_Observation_Actor_Dynamic_ActorCanSeeAgent, AgentCanBeSeenByEnemyObservation },
		{ Key_Observation_Actor_Dynamic_KillDesire, KillDesireObservation },
	};
	
	FLearningAgentsObservationSchemaElement DynamicObservationsObservation = SpecifyOtherCharacterDynamicObservations(InObservationSchema, Settings, AdditionalEnemyObservations, ELAAgentAttitude::Enemy);
	FLearningAgentsObservationSchemaElement StaticObservationsObservation = SpecifyStaticObservations(InObservationSchema, Settings, {} );
	FLearningAgentsObservationSchemaElement CombatHistoryObservations = SpecifyCombatHistoryObservation(InObservationSchema, Settings);
	FLearningAgentsObservationSchemaElement TranslationHistoryObservations = SpecifyTranslationHistoryObservation(InObservationSchema, Settings);
	
	TMap<FName, FLearningAgentsObservationSchemaElement> EnemyObservations = 
	{
		{ Key_Observation_Dynamic, DynamicObservationsObservation },
		{ Key_Observation_Static, StaticObservationsObservation },
		{ Key_Observation_CombatHistory, CombatHistoryObservations },
		{ Key_Observation_TranslationHistory, TranslationHistoryObservations }
	};

	return ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, EnemyObservations, Key_Observation_Enemy);
}

FLearningAgentsObservationSchemaElement ULearningAgentsInteractor_Combat::SpecifyAllyObservations(ULearningAgentsObservationSchema* InObservationSchema,
	const UCombatLearningSettings* Settings) const
{
	FLearningAgentsObservationSchemaElement DynamicObservationsObservation = SpecifyOtherCharacterDynamicObservations(InObservationSchema, Settings, {}, ELAAgentAttitude::Ally);
	FLearningAgentsObservationSchemaElement StaticObservationsObservation = SpecifyStaticObservations(InObservationSchema, Settings, {} );
	FLearningAgentsObservationSchemaElement CombatHistoryObservations = SpecifyCombatHistoryObservation(InObservationSchema, Settings);
	// i don't think translation history is necessary for allies
	// FLearningAgentsObservationSchemaElement TranslationHistoryObservations = SpecifyTranslationHistoryObservation(InObservationSchema, Settings);
	
	TMap<FName, FLearningAgentsObservationSchemaElement> AllyObservations = 
	{
		{ Key_Observation_Dynamic, DynamicObservationsObservation },
		{ Key_Observation_Static, StaticObservationsObservation },
		{ Key_Observation_CombatHistory, CombatHistoryObservations },
		// { Key_Observation_TranslationHistory, TranslationHistoryObservations }
	};

	return ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, AllyObservations, Key_Observation_Ally);
}

TMap<FName, FLearningAgentsObservationSchemaElement> ULearningAgentsInteractor_Combat::SpecifyCombatStateObservation(
	ULearningAgentsObservationSchema* InObservationSchema) const
{
	auto Settings = GetDefault<UCombatLearningSettings>();
	auto CombatTotalDurationObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema,
		Settings->MaxExpectedCombatDuration, Key_Observation_Combat_State_Duration);
	auto EnemiesCountObservation = ULearningAgentsObservations::SpecifyCountObservation(InObservationSchema, Key_Observation_Combat_State_EnemiesCount);
	auto AlliesCountObservation = ULearningAgentsObservations::SpecifyCountObservation(InObservationSchema, Key_Observation_Combat_State_AlliesCount);
	
	TMap<FName, FLearningAgentsObservationSchemaElement> Result =
	{
		{ Key_Observation_Combat_State_Duration, CombatTotalDurationObservation },
		{ Key_Observation_Combat_State_EnemiesCount, EnemiesCountObservation },
		{ Key_Observation_Combat_State_AlliesCount, AlliesCountObservation },
	};
	
	return Result;
}

FLearningAgentsObservationSchemaElement ULearningAgentsInteractor_Combat::SpecifyCombatHistoryObservation(
	ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const
{
	auto EventTypeObservation = ULearningAgentsObservations::SpecifyEnumObservation(InObservationSchema, StaticEnum<ELACombatEvent>(), Key_Observation_CombatHistory_Event_Type);
	auto EventTimeObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, Settings->ExpectedCombatHistoryEventInterval, Key_Observation_CombatHistory_Event_Time);
	auto EventSubjectObservation = ULearningAgentsObservations::SpecifyBoolObservation(InObservationSchema, Key_Observation_CombatHistory_Event_IsSubject);
	auto EventAttitudeToCauserObservation = ULearningAgentsObservations::SpecifyEnumObservation(InObservationSchema, StaticEnum<ELAAgentAttitude>(), Key_Observation_CombatHistory_Event_AttitudeToCauser);
	
	TMap<FName, FLearningAgentsObservationSchemaElement> CombatHistoryItemMap = 
	{
		{ Key_Observation_CombatHistory_Event_Type, EventTypeObservation },
		{ Key_Observation_CombatHistory_Event_Time, EventTimeObservation },
		{ Key_Observation_CombatHistory_Event_IsSubject, EventSubjectObservation },
		{ Key_Observation_CombatHistory_Event_AttitudeToCauser, EventAttitudeToCauserObservation }
	};
	
	FLearningAgentsObservationSchemaElement CombatHistoryObservationElement = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, CombatHistoryItemMap, Key_Observation_CombatHistory_Event);
	return ULearningAgentsObservations::SpecifyStaticArrayObservation(InObservationSchema, CombatHistoryObservationElement, Settings->CombatHistorySize, Key_Observation_CombatHistory_Array);
}

FLearningAgentsObservationSchemaElement ULearningAgentsInteractor_Combat::SpecifyTranslationHistoryObservation(
	ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const
{
	auto VelocityObservation = ULearningAgentsObservations::SpecifyVelocityObservation(InObservationSchema, Settings->MaxSpeed, Key_Observation_TranslationHistory_Velocity);
	auto LocationObservation = ULearningAgentsObservations::SpecifyLocationObservation(InObservationSchema, Settings->EnemyDistanceScale, Key_Observation_TranslationHistory_Entry_Location);
	auto RotationObservation = ULearningAgentsObservations::SpecifyRotationObservation(InObservationSchema, Key_Observation_TranslationHistory_Entry_Rotation);
	TMap<FName, FLearningAgentsObservationSchemaElement> CombatHistoryItemMap = 
	{
		{ Key_Observation_TranslationHistory_Velocity, VelocityObservation },
		{ Key_Observation_TranslationHistory_Entry_Location, LocationObservation },
		{ Key_Observation_TranslationHistory_Entry_Rotation, RotationObservation }
	};
	
	FLearningAgentsObservationSchemaElement TranslationHistoryObservationElement = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, CombatHistoryItemMap, Key_Observation_TranslationHistory_Entry);
	return ULearningAgentsObservations::SpecifyStaticArrayObservation(InObservationSchema, TranslationHistoryObservationElement, Settings->TranslationHistorySize, Key_Observation_TranslationHistory_Array);
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GatherOptionalNamedExclusiveObservation(ULearningAgentsObservationObject* InObservationObject, const FName& ObservationName,
                                                                                                                  const FName& ObservationOptionalWrapperName, const FGameplayTag& OptionTag,
                                                                                                                  const UCombatLearningSettings* Settings, const FVector& AgentWorldLocation, int AgentId)
{
	FLearningAgentsObservationObjectElement Observation;
	if (OptionTag.IsValid())
	{
		Observation = ULearningAgentsObservations::MakeNamedExclusiveDiscreteObservation(InObservationObject,
			OptionTag.GetTagName(), ObservationName,
			Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
	}
	
	ELearningAgentsOptionalObservation OptionalObservationStatus = OptionTag.IsValid()
       ? ELearningAgentsOptionalObservation::Valid
       : ELearningAgentsOptionalObservation::Null;
	
	auto OptionalObservation = ULearningAgentsObservations::MakeOptionalObservation(InObservationObject, Observation, OptionalObservationStatus, ObservationOptionalWrapperName);
	return OptionalObservation;
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GatherStaticObservations(ULearningAgentsObservationObject* InObservationObject, int AgentId,
	const FCharacterDataBase& CharacterData, const UCombatLearningSettings* Settings, const FVector& AgentWorldLocation,
	const TMap<FName, FLearningAgentsObservationObjectElement>& AdditionalObservations)
{
	auto LevelObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, CharacterData.Level, Key_Observation_Static_Level,
	    Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
	auto ArmorRateObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, CharacterData.ArmorRate, Key_Observation_Static_ArmorRate,
		Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
	
	TMap<FName, FLearningAgentsObservationObjectElement> StaticSelfObservations =
	{
		{ Key_Observation_Static_Level, LevelObservation },
		{ Key_Observation_Static_ArmorRate, ArmorRateObservation },
	};

	StaticSelfObservations.Append(AdditionalObservations);
	
	auto StaticSelfObservationsObservation = ULearningAgentsObservations::MakeStructObservation(InObservationObject, StaticSelfObservations, Key_Observation_Static);
	return StaticSelfObservationsObservation;
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GatherDynamicObservations(ULearningAgentsObservationObject* InObservationObject, int AgentId, const FCharacterDataBase& CharacterData, 
	const UCombatLearningSettings* Settings, const  TMap<FName, FLearningAgentsObservationObjectElement>& AdditionalObservations,
	const FVector& AgentWorldLocation, const FTransform& AgentTransform)
{
	auto HealthObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, CharacterData.NormalizedHealth, Key_Observation_Dynamic_Health,
	                                                                           Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
	auto AccumulatedDamageObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, CharacterData.AccumulatedNormalizedDamage, Key_Observation_Dynamic_AccumulatedDamage,
																			   Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
	auto VelocityObservation = ULearningAgentsObservations::MakeVelocityObservation(InObservationObject, CharacterData.WorldVelocity, AgentTransform,
		Key_Observation_Dynamic_Velocity,
		Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
	
	auto StatesObservation = ULearningAgentsObservations::MakeBitmaskObservation(InObservationObject,
		StaticEnum<ELACharacterStates>(), static_cast<int32>(CharacterData.CombatStates), Key_Observation_Dynamic_CombatStates,
		Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);

	auto WeaponObservation = GetWeaponObservation(InObservationObject, AgentId, CharacterData.WeaponData, Settings, AgentWorldLocation);
	
	TMap<FName, FLearningAgentsObservationObjectElement> DynamicObservations =
	{
		{ Key_Observation_Dynamic_Health, HealthObservation },
		{ Key_Observation_Dynamic_AccumulatedDamage, AccumulatedDamageObservation },
		{ Key_Observation_Dynamic_Velocity, VelocityObservation },
		{ Key_Observation_Dynamic_CombatStates, StatesObservation },
		{ Key_Observation_ActiveWeapon, WeaponObservation },
	};
	
	if (!Settings->Gestures.IsEmpty())
	{
		FLearningAgentsObservationObjectElement OptionalObservation = GatherOptionalNamedExclusiveObservation(InObservationObject, 
			Key_Observation_Dynamic_Gesture, Key_Observation_Dynamic_Gesture_Optional, CharacterData.ActiveGesture, 
			Settings, AgentWorldLocation, AgentId);
		
		DynamicObservations.Add(Key_Observation_Dynamic_Gesture_Optional, OptionalObservation);
	}
	
	if (!Settings->Phrases.IsEmpty())
	{
		FLearningAgentsObservationObjectElement OptionalObservation = GatherOptionalNamedExclusiveObservation(InObservationObject, 
			Key_Observation_Dynamic_Phrase, Key_Observation_Dynamic_Phrase_Optional, CharacterData.ActivePhrase, 
			Settings, AgentWorldLocation, AgentId);
		
		DynamicObservations.Add(Key_Observation_Dynamic_Phrase_Optional, OptionalObservation);
	}
	
	DynamicObservations.Append(AdditionalObservations);
	return ULearningAgentsObservations::MakeStructObservation(InObservationObject, DynamicObservations, Key_Observation_Dynamic);
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GatherCombatHistoryObservation(const FLAObservationGatherParams& GatheringParams,
	const ULAObservationHistoryComponent* ObservationHistoryComponent)
{
	const auto& CombatHistory = ObservationHistoryComponent->GetCombatHistory();
	TArray<FLearningAgentsObservationObjectElement> CombatHistoryObservations;
	CombatHistoryObservations.Reserve(GatheringParams.Settings->CombatHistorySize);
	const float WorldTime = GetWorld()->GetTimeSeconds();
	for (int i = 0; i < CombatHistory.Num(); ++i)
	{
		const float TimeSinceEvent = CombatHistory[i].AtWorldTime > 0.f ? WorldTime - CombatHistory[i].AtWorldTime : CombatHistory[i].AtWorldTime;
		auto EventTypeObservation = ULearningAgentsObservations::MakeEnumObservation(GatheringParams.InObservationObject, StaticEnum<ELACombatEvent>(), static_cast<uint8>(CombatHistory[i].CombatEvent),
			Key_Observation_CombatHistory_Event_Type, 
			GatheringParams.Settings->bVisLogEnabled, this, GatheringParams.AgentId, GatheringParams.AgentWorldLocation);
		auto EventTimeObservation = ULearningAgentsObservations::MakeFloatObservation(GatheringParams.InObservationObject, TimeSinceEvent, Key_Observation_CombatHistory_Event_Time,
			GatheringParams.Settings->bVisLogEnabled, this, GatheringParams.AgentId, GatheringParams.AgentWorldLocation);
		auto EventSubjectObservation = ULearningAgentsObservations::MakeBoolObservation(GatheringParams.InObservationObject, CombatHistory[i].bEventSubject, Key_Observation_CombatHistory_Event_IsSubject,
			GatheringParams.Settings->bVisLogEnabled, this, GatheringParams.AgentId, GatheringParams.AgentWorldLocation);
		auto EventAttitudeToCauserObservation = ULearningAgentsObservations::MakeEnumObservation(GatheringParams.InObservationObject, StaticEnum<ELAAgentAttitude>(), CombatHistory[i].AtWorldTime, Key_Observation_CombatHistory_Event_IsSubject,
			GatheringParams.Settings->bVisLogEnabled, this, GatheringParams.AgentId, GatheringParams.AgentWorldLocation);
		
		TMap<FName, FLearningAgentsObservationObjectElement> CombatHistoryItemMap = 
		{
			{ Key_Observation_CombatHistory_Event_Type, EventTypeObservation },
			{ Key_Observation_CombatHistory_Event_Time, EventTimeObservation },
			{ Key_Observation_CombatHistory_Event_IsSubject, EventSubjectObservation }
		};
	
		auto CombatHistoryObservationItem = ULearningAgentsObservations::MakeStructObservation(GatheringParams.InObservationObject, CombatHistoryItemMap, Key_Observation_CombatHistory_Event);
		CombatHistoryObservations.Add(CombatHistoryObservationItem);
	}
	
	auto CombatHistoryObservation = ULearningAgentsObservations::MakeStaticArrayObservation(GatheringParams.InObservationObject, CombatHistoryObservations, Key_Observation_CombatHistory_Array);
	return CombatHistoryObservation;
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GatherTranslationHistoryObservation(const FLAObservationGatherParams& GatheringParams, 
	const ULAObservationHistoryComponent* ObservationHistoryComponent)
{
	auto TranslationHistory = ObservationHistoryComponent->GetTranslationHistory(GatheringParams.AgentActor);
	TArray<FLearningAgentsObservationObjectElement> TranslationHistoryObservations;
	TranslationHistoryObservations.Reserve(GatheringParams.Settings->TranslationHistorySize);
	for (int i = 0; i < TranslationHistory.Num(); i++)
	{
		auto VelocityObservation = ULearningAgentsObservations::MakeVelocityObservation(GatheringParams.InObservationObject, TranslationHistory[i].Velocity, TranslationHistory[i].RelativeTransform,
			Key_Observation_TranslationHistory_Velocity,
			GatheringParams.Settings->bVisLogEnabled, this, GatheringParams.AgentId, GatheringParams.AgentWorldLocation);
		auto LocationObservation = ULearningAgentsObservations::MakeLocationObservation(GatheringParams.InObservationObject, TranslationHistory[i].Location, TranslationHistory[i].RelativeTransform,
			Key_Observation_TranslationHistory_Entry_Location,
			GatheringParams.Settings->bVisLogEnabled, this, GatheringParams.AgentId, GatheringParams.AgentWorldLocation);
		auto RotationObservation = ULearningAgentsObservations::MakeRotationObservation(GatheringParams.InObservationObject, TranslationHistory[i].Rotation, TranslationHistory[i].RelativeTransform.Rotator(),
			Key_Observation_TranslationHistory_Entry_Rotation,
			GatheringParams.Settings->bVisLogEnabled, this, GatheringParams.AgentId, GatheringParams.AgentWorldLocation);
		TMap<FName, FLearningAgentsObservationObjectElement> CombatHistoryItemMap = 
		{
			{ Key_Observation_TranslationHistory_Velocity, VelocityObservation },
			{ Key_Observation_TranslationHistory_Entry_Location, LocationObservation },
			{ Key_Observation_TranslationHistory_Entry_Rotation, RotationObservation }
		};
	
		auto TranslationHistoryObservationElement = ULearningAgentsObservations::MakeStructObservation(GatheringParams.InObservationObject, CombatHistoryItemMap, 
			Key_Observation_TranslationHistory_Entry);
		TranslationHistoryObservations.Add(TranslationHistoryObservationElement);
	}

	auto TranslationHistoryObservation = ULearningAgentsObservations::MakeStaticArrayObservation(GatheringParams.InObservationObject, TranslationHistoryObservations, Key_Observation_TranslationHistory_Array);
	return TranslationHistoryObservation;
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GatherSelfObservations(
	ULearningAgentsObservationObject* InObservationObject, int AgentId, const FSelfData& SelfData)
{
	auto Settings = GetDefault<UCombatLearningSettings>();
	auto AgentActor = Cast<AActor>(GetAgent(AgentId));
	auto AgentWorldLocation = AgentActor->GetActorLocation();
	auto AgentTransform = AgentActor->GetTransform();
	
	FLAObservationGatherParams GatheringParams 
	{
		InObservationObject, Settings, AgentActor, AgentWorldLocation, AgentTransform, AgentId,
	};
	
// ==================== gather static observations
	auto SurvivalDesireObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, SelfData.SurvivalDesire, Key_Observation_Static_SurvivalDesire,
		Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
	TMap<FName, FLearningAgentsObservationObjectElement> AdditionalStaticObservations = 
	{
		{ Key_Observation_Static_SurvivalDesire, SurvivalDesireObservation }
	};
	
	FLearningAgentsObservationObjectElement StaticSelfObservationsObservation = GatherStaticObservations(InObservationObject, AgentId, SelfData, Settings, AgentWorldLocation, AdditionalStaticObservations);
	
// ====================== gather dynamic observations 

	auto StaminaObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, SelfData.NormalizedStamina, Key_Observation_Dynamic_Stamina,
		Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
	TMap<FName, FLearningAgentsObservationObjectElement> AdditionalDynamicObservations =
	{
		{ Key_Observation_Dynamic_Stamina, StaminaObservation },
	};
	
	FLearningAgentsObservationObjectElement DynamicSelfObservationsObservation = GatherDynamicObservations(InObservationObject, AgentId, SelfData, Settings, AdditionalDynamicObservations,
		AgentWorldLocation, AgentTransform);
	
	ULAObservationHistoryComponent* ObservationHistoryComponent = AgentActor->FindComponentByClass<ULAObservationHistoryComponent>();
	
	FLearningAgentsObservationObjectElement CombatHistoryObservation = GatherCombatHistoryObservation(GatheringParams, ObservationHistoryComponent);
	FLearningAgentsObservationObjectElement TranslationHistoryObservation = GatherTranslationHistoryObservation(GatheringParams, ObservationHistoryComponent);
	
// ====================== combine static and dynamic observations
	
	TMap<FName, FLearningAgentsObservationObjectElement> Observations =
	{
		{ Key_Observation_Static, StaticSelfObservationsObservation },
		{ Key_Observation_Dynamic, DynamicSelfObservationsObservation },
		{ Key_Observation_CombatHistory, CombatHistoryObservation },
		{ Key_Observation_TranslationHistory, TranslationHistoryObservation }
	};
	
	return ULearningAgentsObservations::MakeStructObservation(InObservationObject, Observations, Key_Observation_Self);
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GatherSurroundingsObservations(ULearningAgentsObservationObject* InObservationObject,
	int32 AgentId, ULearningAgentCombatObservationComponent* LAObservationComponent)
{
	// welp, until there's no conv3d, LAs will observe convolved "heightmaps" in different plains 
	TMap<FName, FLearningAgentsObservationObjectElement> ResultMap;
	
	const FLidarSelfObservationCache& LidarData = LAObservationComponent->GetSelfLidarData();
	auto CeilingObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, LidarData.AverageCeilingHeight,
		Key_Observation_Surrounding_LIDAR_Ceiling);
	
	TArray<FLearningAgentsObservationObjectElement> RaindropDownwardObservations;
	RaindropDownwardObservations.SetNum(LidarData.DownwardRaindrops->Num());
	for (int i = 0; i < LidarData.DownwardRaindrops->Num(); i++)
	{
		auto RaindropDownHeightmapItemObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject,
			(*LidarData.DownwardRaindrops)[i], Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Item);
		RaindropDownwardObservations[i] = RaindropDownHeightmapItemObservation;
	}
	
	auto RaindropDownwardObservationsObservation = ULearningAgentsObservations::MakeStaticArrayObservation(InObservationObject, 
		RaindropDownwardObservations, Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Array);
	auto RaindropDownwardObservationsConvolved = ULearningAgentsObservations::MakeConv2dObservation(InObservationObject,
		RaindropDownwardObservationsObservation, Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Convolved);
	
	TArray<FLearningAgentsObservationObjectElement> RaindropForwardObservations;
	RaindropForwardObservations.SetNum(LidarData.ForwardRaindrops->Num());
	for (int i = 0; i < LidarData.ForwardRaindrops->Num(); i++)
	{
		auto RaindropForwardHeightmapItemObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject,
			(*LidarData.ForwardRaindrops)[i], Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Item);
		RaindropForwardObservations[i] = RaindropForwardHeightmapItemObservation;
	}
	
	auto RaindropForwardObservationsObservation = ULearningAgentsObservations::MakeStaticArrayObservation(InObservationObject, 
		RaindropForwardObservations, Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Array);
	auto RaindropForwardObservationsConvolved = ULearningAgentsObservations::MakeConv2dObservation(InObservationObject,
		RaindropForwardObservationsObservation, Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Convolved);
	
	TArray<FLearningAgentsObservationObjectElement> RaindropBackwardObservations;
	RaindropBackwardObservations.SetNum(LidarData.BackwardRaindrops->Num());
	for (int i = 0; i < LidarData.BackwardRaindrops->Num(); i++)
	{
		auto RaindropBackwardHeightmapItemObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject,
			(*LidarData.BackwardRaindrops)[i], Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Item);
		RaindropBackwardObservations[i] = RaindropBackwardHeightmapItemObservation;
	}
	
	auto RaindropBackwardObservationsObservation = ULearningAgentsObservations::MakeStaticArrayObservation(InObservationObject, 
		RaindropBackwardObservations, Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Array);
	auto RaindropBackwardObservationsConvolved = ULearningAgentsObservations::MakeConv2dObservation(InObservationObject,
		RaindropBackwardObservationsObservation, Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Convolved);
	
	ResultMap = 
	{
		{ Key_Observation_Surrounding_LIDAR_Ceiling, CeilingObservation },
		{ Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Convolved, RaindropDownwardObservationsConvolved },
		{ Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Convolved, RaindropForwardObservationsConvolved },
		{ Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Convolved, RaindropBackwardObservationsConvolved },
	};
	
	auto Result = ULearningAgentsObservations::MakeStructObservation(InObservationObject, ResultMap, Key_Observation_Surrounding_LIDAR);
	return Result;
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GatherCombatStateObservation(ULearningAgentsObservationObject* InObservationObject,
	int32 AgentId, const FCombatStateData& CombatStateData)
{
	auto Settings = GetDefault<UCombatLearningSettings>();
	auto AgentActor = Cast<AActor>(GetAgent(AgentId));
	auto AgentWorldLocation = AgentActor->GetActorLocation();
	
	auto CombatTotalDurationObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, CombatStateData.CombatTotalDuration,
		Key_Observation_Combat_State_Duration,
		Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
	auto EnemiesCountObservation = ULearningAgentsObservations::MakeCountObservation(InObservationObject, 
		CombatStateData.EnemiesCount, Settings->MaxEnemiesObservedAtOnce, Key_Observation_Combat_State_EnemiesCount,
		Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
	auto AlliesCountObservation = ULearningAgentsObservations::MakeCountObservation(InObservationObject, 
		CombatStateData.AlliesCount, Settings->MaxAlliesObservedAtOnce, Key_Observation_Combat_State_AlliesCount,
		Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
	TMap<FName, FLearningAgentsObservationObjectElement> Observations =
	{
		{ Key_Observation_Combat_State_Duration, CombatTotalDurationObservation },
		{ Key_Observation_Combat_State_EnemiesCount, EnemiesCountObservation },
		{ Key_Observation_Combat_State_AlliesCount, AlliesCountObservation }
	};
	
	return ULearningAgentsObservations::MakeStructObservation(InObservationObject, Observations, Key_Observation_Combat_State);
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GatherOtherCharacterDynamicObservations(ULearningAgentsObservationObject* InObservationObject,
	int32 AgentId, const UCombatLearningSettings* Settings, const TArray<float>* RaindropsTo,
	const FVector& AgentWorldLocation, const FTransform& AgentTransform, const FRotator& AgentRotation,
	const FPerceivedCharacterData& ActorState, const TMap<FName, FLearningAgentsObservationObjectElement>& AdditionalObservations)
{
	auto ActorAliveObservation = ULearningAgentsObservations::MakeBoolObservation(InObservationObject, ActorState.bAlive,
		Key_Observation_Actor_Dynamic_IsAlive,
		Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
	auto ActorCanSeeEnemyObservation = ULearningAgentsObservations::MakeBoolObservation(InObservationObject, ActorState.bAgentCanSeeCharacter,
		Key_Observation_Actor_Dynamic_AgentCanSeeActor,
		Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
		
	// these 2 might be redundant as there's translation history observation
	auto ActorLocationObservation = ULearningAgentsObservations::MakeLocationObservation(InObservationObject,
		ActorState.Actor->GetActorLocation(), AgentTransform,
		Key_Observation_Actor_Dynamic_Location,
		Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
	auto ActorOrientationObservation = ULearningAgentsObservations::MakeRotationObservation(InObservationObject,
		ActorState.Actor->GetActorRotation(), AgentRotation,
		Key_Observation_Actor_Dynamic_Orientation,
		Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
	
	
	FLearningAgentsObservationObjectElement RaindropToActorOptionalObservation;
	if (RaindropsTo != nullptr)
	{
		TArray<FLearningAgentsObservationObjectElement> RaindropToActorLidarObservations;
		RaindropToActorLidarObservations.SetNum(RaindropsTo->Num());
		for (int i = 0; i < RaindropsTo->Num(); i++)
		{
			auto RaindropToEnemyLidarObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject,
				(*RaindropsTo)[i], Key_Observation_Actor_Dynamic_RaindropTo_Lidar);
			RaindropToActorLidarObservations[i] = RaindropToEnemyLidarObservation;
		}
	
		auto RaindropToEnemyArrayObservation = ULearningAgentsObservations::MakeStaticArrayObservation(InObservationObject, 
			RaindropToActorLidarObservations, Key_Observation_Actor_Dynamic_RaindropTo_Array);
		auto RaindropToEnemyObservationsConvolved = ULearningAgentsObservations::MakeConv2dObservation(InObservationObject,
			RaindropToEnemyArrayObservation, Key_Observation_Actor_Dynamic_RaindropTo_Convolved);
		RaindropToActorOptionalObservation = ULearningAgentsObservations::MakeOptionalValidObservation(InObservationObject, RaindropToEnemyObservationsConvolved, 
			Key_Observation_Actor_Dynamic_RaindropTo_Convolved_Optional);
	}
	else
	{
		RaindropToActorOptionalObservation = ULearningAgentsObservations::MakeOptionalNullObservation(InObservationObject,
			Key_Observation_Actor_Dynamic_RaindropTo_Convolved_Optional);
	}

	TMap<FName, FLearningAgentsObservationObjectElement> AdditionalOtherCharacterObservations = 
	{
		{ Key_Observation_Actor_Dynamic_IsAlive, ActorAliveObservation},
		{ Key_Observation_Actor_Dynamic_Location, ActorLocationObservation },
		{ Key_Observation_Actor_Dynamic_Orientation, ActorOrientationObservation },
		{ Key_Observation_Actor_Dynamic_AgentCanSeeActor, ActorCanSeeEnemyObservation },
		{ Key_Observation_Actor_Dynamic_RaindropTo_Convolved_Optional, RaindropToActorOptionalObservation }
	};
		
	AdditionalOtherCharacterObservations.Append(AdditionalObservations);
	auto DynamicObservationsObservation = GatherDynamicObservations(InObservationObject, AgentId, ActorState, Settings, AdditionalOtherCharacterObservations, AgentWorldLocation, AgentTransform);
	return DynamicObservationsObservation;
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GatherEnemiesObservation(ULearningAgentsObservationObject* InObservationObject,
                                                                                                   ULearningAgentCombatObservationComponent* CombatObservationComponent, int32 AgentId)
{
	auto Settings = GetDefault<UCombatLearningSettings>();
	auto AgentActor = Cast<AActor>(GetAgent(AgentId));
	auto AgentWorldLocation = AgentActor->GetActorLocation();
	auto AgentTransform = AgentActor->GetActorTransform();
	auto AgentRotation = AgentActor->GetActorRotation();

	FLAObservationGatherParams GatheringParams 
	{
		InObservationObject, Settings, AgentActor, AgentWorldLocation, AgentTransform, AgentId,
	};
	
	auto EnemiesObservationData = CombatObservationComponent->GetEnemiesObservationData();
	TArray<FLearningAgentsObservationObjectElement> EnemiesObservations;
	
	for (const auto& EnemyObservationData : EnemiesObservationData)
	{
		const auto EnemyState = StaticCastSharedRef<FEnemyData>(EnemyObservationData.Value->CharacterState);

		auto EnemyCanSeeAgentObservation = ULearningAgentsObservations::MakeBoolObservation(InObservationObject, EnemyState->bCharacterCanSeeAgent,
			Key_Observation_Actor_Dynamic_ActorCanSeeAgent,
			Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
		auto KillDesireObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, EnemyState->KillDesire,
			Key_Observation_Actor_Dynamic_KillDesire,
			Settings->bVisLogEnabled, this, AgentId, AgentWorldLocation);
		
		TMap<FName, FLearningAgentsObservationObjectElement> AdditionalEnemyObservations = 
		{
			{ Key_Observation_Actor_Dynamic_ActorCanSeeAgent, EnemyCanSeeAgentObservation },
			{ Key_Observation_Actor_Dynamic_KillDesire, KillDesireObservation },
		};

		const TArray<float>* RaindropsTo = CombatObservationComponent->GetLidarDataTo(EnemyState->Actor.Get(), ELAAgentAttitude::Enemy);
		auto DynamicObservationsObservation = GatherOtherCharacterDynamicObservations(InObservationObject, AgentId, Settings, RaindropsTo,
			AgentWorldLocation, AgentTransform, AgentRotation, EnemyState.Get(), AdditionalEnemyObservations);
		
		auto StaticObservationsObservation = GatherStaticObservations(InObservationObject, AgentId, EnemyState.Get(), Settings, AgentWorldLocation, {});
		
		ULAObservationHistoryComponent* ObservationHistoryComponent = EnemyState->Actor->FindComponentByClass<ULAObservationHistoryComponent>();
		FLearningAgentsObservationObjectElement CombatHistoryObservation = GatherCombatHistoryObservation(GatheringParams, ObservationHistoryComponent);
		FLearningAgentsObservationObjectElement TranslationHistoryObservation = GatherTranslationHistoryObservation(GatheringParams, ObservationHistoryComponent);
		
		TMap<FName, FLearningAgentsObservationObjectElement> ActorObservationsCategories = 
		{
			{ Key_Observation_Dynamic, DynamicObservationsObservation },
			{ Key_Observation_Static, StaticObservationsObservation },
			{ Key_Observation_CombatHistory, CombatHistoryObservation },
			{ Key_Observation_TranslationHistory, TranslationHistoryObservation }
		};
		
		auto EnemyObservationsObservation = ULearningAgentsObservations::MakeStructObservation(InObservationObject, ActorObservationsCategories, Key_Observation_Enemy);
		EnemiesObservations.Add(EnemyObservationsObservation);
	}

	return ULearningAgentsObservations::MakeArrayObservation(InObservationObject, EnemiesObservations, Settings->MaxEnemiesObservedAtOnce, Key_Observation_Enemies);
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GatherAlliesObservations(ULearningAgentsObservationObject* InObservationObject,
	ULearningAgentCombatObservationComponent* CombatObservationComponent, int32 AgentId)
{
	auto Settings = GetDefault<UCombatLearningSettings>();
	auto AgentActor = Cast<AActor>(GetAgent(AgentId));
	auto AgentWorldLocation = AgentActor->GetActorLocation();
	auto AgentTransform = AgentActor->GetActorTransform();
	auto AgentRotation = AgentActor->GetActorRotation();

	FLAObservationGatherParams GatheringParams 
	{
		InObservationObject, Settings, AgentActor, AgentWorldLocation, AgentTransform, AgentId,
	};
	
	TArray<FLearningAgentsObservationObjectElement> AlliesObservations;
	// auto AlliesData = CombatObservationComponent->GetAllies();
	const auto& AlliesObservationData = CombatObservationComponent->GetAlliesObservationData();
	
	for (const auto& AllyObservationData : AlliesObservationData)
	{
		const auto AllyState = StaticCastSharedRef<FAllyData>(AllyObservationData.Value->CharacterState);
		const TArray<float>* RaindropsTo = CombatObservationComponent->GetLidarDataTo(AllyState->Actor.Get(), ELAAgentAttitude::Ally);
		
		auto DynamicObservationsObservation = GatherOtherCharacterDynamicObservations(InObservationObject, AgentId, Settings, RaindropsTo,
			AgentWorldLocation, AgentTransform, AgentRotation, AllyState.Get(), {});
		auto StaticObservationsObservation = GatherStaticObservations(InObservationObject, AgentId, AllyState.Get(), Settings, AgentWorldLocation, {});

		ULAObservationHistoryComponent* ObservationHistoryComponent = AllyState->Actor->FindComponentByClass<ULAObservationHistoryComponent>();
		FLearningAgentsObservationObjectElement CombatHistoryObservation = GatherCombatHistoryObservation(GatheringParams, ObservationHistoryComponent);
		
		TMap<FName, FLearningAgentsObservationObjectElement> AllyObservations = 
		{
			{ Key_Observation_Dynamic, DynamicObservationsObservation },
			{ Key_Observation_Static, StaticObservationsObservation },
			{ Key_Observation_CombatHistory, CombatHistoryObservation }
		};

		auto AllyObservationsObservation = ULearningAgentsObservations::MakeStructObservation(InObservationObject, AllyObservations,Key_Observation_Ally);
		AlliesObservations.Add(AllyObservationsObservation);
	}

	return ULearningAgentsObservations::MakeArrayObservation(InObservationObject, AlliesObservations, Settings->MaxAlliesObservedAtOnce, Key_Observation_Allies);
}

FLearningAgentsObservationSchemaElement ULearningAgentsInteractor_Combat::SpecifyNamedExclusiveDiscreteObservation(
	ULearningAgentsObservationSchema* InObservationSchema, const TMap<FGameplayTag, float>& ObservationOptions,
	const FName& ObservationName, const FName& ObservationOptionalWrapperName) const
{
	TArray<FName> OptionNames;
	OptionNames.Reserve(ObservationOptions.Num());
	for (const auto& Options : ObservationOptions)
		OptionNames.Emplace(Options.Key.GetTagName());
		
	auto Observation = ULearningAgentsObservations::SpecifyNamedExclusiveDiscreteObservation(InObservationSchema, OptionNames, ObservationName);
	
	return ULearningAgentsObservations::SpecifyOptionalObservation(InObservationSchema, Observation, 32, ObservationOptionalWrapperName);
}

bool ULearningAgentsInteractor_Combat::GetSelfStates(const ULearningAgentsObservationObject* InObservationObject, const FLearningAgentsObservationObjectElement& InObservationObjectElement,
                                                     ELACharacterStates& OutSelfStates) const
{
	TMap<FName, FLearningAgentsObservationObjectElement> ObservationsRootMap;
	bool bFoundObservationRoot = ULearningAgentsObservations::GetStructObservation(ObservationsRootMap, InObservationObject,
		InObservationObjectElement, Key_CombatObservations);
	if (!bFoundObservationRoot)
	{
		ensure(false);
		return false;
	}
	
	auto SelfObservationsObservation = ObservationsRootMap.Find(Key_Observation_Self);
	if (SelfObservationsObservation == nullptr)
	{
		ensure(false);
		return false;
	}
	
	TMap<FName, FLearningAgentsObservationObjectElement> DynamicSelfObservations;
	bool bFoundDynamicSelfObservation = ULearningAgentsObservations::GetStructObservation(DynamicSelfObservations, InObservationObject, *SelfObservationsObservation,
		Key_Observation_Dynamic);
	if (!bFoundDynamicSelfObservation)
	{
		ensure(false);
		return false;
	}
	
	auto SelfStatesObservation = DynamicSelfObservations.Find(Key_Observation_Dynamic_CombatStates);
	if (SelfStatesObservation == nullptr)
	{
		ensure(false);
		return false;
	}

	OutSelfStates = ELACharacterStates::None;
	int32 RawSelfStates = 0;
	bool bFoundSelfStates = ULearningAgentsObservations::GetBitmaskObservation(RawSelfStates, InObservationObject, *SelfStatesObservation,
																			   StaticEnum<ELACharacterStates>(), Key_Observation_Dynamic_CombatStates);
	if (!bFoundSelfStates)
	{
		ensure(false);
		return false;
	}
	
	OutSelfStates = static_cast<ELACharacterStates>(RawSelfStates);
	return true;
}

TSet<FName> ULearningAgentsInteractor_Combat::GetMaskedActions(ELACharacterStates SelfStates) const
{
	TSet<FName> Result;
	Result.Reserve(16);
	if ((SelfStates & ELACharacterStates::Attacking) != ELACharacterStates::None)
	{
		// idk, it can actually depend on the archetype 
		Result.Emplace(Key_Action_Combat_Dodge);
		Result.Emplace(Key_Action_Locomotion);
	}
	
	if ((SelfStates & ELACharacterStates::InStagger) != ELACharacterStates::None)
	{
		Result.Emplace(Key_Action_Locomotion);
		Result.Emplace(Key_Action_Combat);
	}
	
	if ((SelfStates & ELACharacterStates::Blocking) != ELACharacterStates::None)
	{
		Result.Emplace(Key_Action_Gesture);
		Result.Emplace(Key_Action_UseConsumableItem);
		Result.Emplace(Key_Action_ChangeWeaponState);
	}
	
	if ((SelfStates & ELACharacterStates::Dodging) != ELACharacterStates::None)
	{
		Result.Emplace(Key_Action_Combat_Attack);
		Result.Emplace(Key_Action_Locomotion);
	}
	
	if ((SelfStates & ELACharacterStates::Dying) != ELACharacterStates::None)
	{
		Result.Emplace(Key_Action_Locomotion_Blocking);
		Result.Emplace(Key_Action_Combat_Block);
		Result.Emplace(Key_Action_Combat_Dodge);
	}
	
	if ((SelfStates & ELACharacterStates::InAir) != ELACharacterStates::None)
	{
		Result.Emplace(Key_Action_Combat_Dodge);
		Result.Emplace(Key_Action_Locomotion_SetSpeed);
		Result.Emplace(Key_Action_Locomotion_Move);
	}
	
	if ((SelfStates & ELACharacterStates::Swimming) != ELACharacterStates::None)
	{
		Result.Emplace(Key_Action_Combat);
		Result.Emplace(Key_Action_Gesture);
		Result.Emplace(Key_Action_ChangeWeaponState);
	}
	
	if ((SelfStates & ELACharacterStates::WeaponNotReady) != ELACharacterStates::None)
	{
		Result.Emplace(Key_Action_Combat_Attack);	
		Result.Emplace(Key_Action_Combat_Block);	
	}
	
	if ((SelfStates & ELACharacterStates::Gesturing) != ELACharacterStates::None)
	{
		Result.Emplace(Key_Action_Gesture);
	}

	if ((SelfStates & ELACharacterStates::Speaking) != ELACharacterStates::None)
	{
		Result.Emplace(Key_Action_SayPhrase);
	}
	
	if ((SelfStates & ELACharacterStates::UsingConsumableItem) != ELACharacterStates::None)
	{
		Result.Emplace(Key_Action_SayPhrase);
		Result.Emplace(Key_Action_Gesture);
		Result.Emplace(Key_Action_Locomotion_Blocking);
		Result.Emplace(Key_Action_ChangeWeaponState);
	}
	
	if ((SelfStates & ELACharacterStates::HasActiveRootMotionAnimation) != ELACharacterStates::None)
	{
		Result.Emplace(Key_Action_Locomotion_Move);
		Result.Emplace(Key_Action_Locomotion_SetSpeed);
		Result.Emplace(Key_Action_Locomotion_NonBlocking_Animation);
	}
	
	if ((SelfStates & ELACharacterStates::InteractingWithObject) != ELACharacterStates::None)
	{
		Result.Emplace(Key_Action_Locomotion);
		Result.Emplace(Key_Action_Locomotion_NonBlocking_Animation);
	}
	
	if ((SelfStates & ELACharacterStates::ChangingWeaponState) != ELACharacterStates::None)
	{
		Result.Emplace(Key_Action_Locomotion_NonBlocking_Animation);
	}
	
	return Result;
}

TArray<uint8> ULearningAgentsInteractor_Combat::GetBlockingLocomotionActionsMask(ELACharacterStates SelfStates) const
{
	TSet<uint8> Result;
	if ((SelfStates & ELACharacterStates::Swimming) != ELACharacterStates::None)
		Result.Add(static_cast<uint8>(ELALocomotionAction::Jump));	
	
	if ((SelfStates & ELACharacterStates::ChangingWeaponState) != ELACharacterStates::None)
		Result.Add(static_cast<uint8>(ELALocomotionAction::Mantle));
	
	if ((SelfStates & ELACharacterStates::UsingConsumableItem) != ELACharacterStates::None)
		Result.Add(static_cast<uint8>(ELALocomotionAction::Mantle));
	
	return Result.Array();
}

TArray<uint8> ULearningAgentsInteractor_Combat::GetWeaponStateChangeMask(ELACharacterStates SelfStates) const
{
	TSet<uint8> Result;
	if ((SelfStates & ELACharacterStates::Swimming) != ELACharacterStates::None)
		Result.Add(static_cast<uint8>(ELAWeaponStateChange::Ready));	
	
	if ((SelfStates & ELACharacterStates::WeaponReady) != ELACharacterStates::None)
		Result.Add(static_cast<uint8>(ELAWeaponStateChange::Ready));
	
	if ((SelfStates & ELACharacterStates::WeaponNotReady) != ELACharacterStates::None)
		Result.Add(static_cast<uint8>(ELAWeaponStateChange::Unready));
	
	return Result.Array();
}

void ULearningAgentsInteractor_Combat::SampleCombatAction(const ULearningAgentsActionObject* InActionObject,
                                                          const int32 AgentId, const AActor* AgentActor, ULearningAgentCombatActionsComponent* CombatActionsComponent,
                                                          const FLearningAgentsActionObjectElement& RootActionObjectElement)
{
	auto AgentLocation = AgentActor->GetActorLocation();
	auto AgentTransform  = AgentActor->GetTransform();
	auto Settings = GetDefault<UCombatLearningSettings>();
	
	FName CombatActionName;
	FLearningAgentsActionObjectElement CombatActionObjectElement;
	bool bGetActionSuccess = ULearningAgentsActions::GetExclusiveUnionAction(CombatActionName, CombatActionObjectElement, InActionObject, 
	RootActionObjectElement, Key_Action_Combat);
	if (!ensure(bGetActionSuccess))
		return;
	
	if (CombatActionName == Key_Action_Combat_Attack)
	{
		uint8 AttackIndex = 0;
		bool bSuccess = ULearningAgentsActions::GetEnumAction(AttackIndex, 
			InActionObject, CombatActionObjectElement, GetAttackEnum(), Key_Action_Combat_Attack,
			Settings->bVisLogEnabled, this, AgentId, AgentLocation);
		if (ensure(bSuccess))
			CombatActionsComponent->Attack(AttackIndex);
	}
	else if (CombatActionName == Key_Action_Combat_Block)
	{
		float ParryAngle = 0.f;
		bool bSuccess = ULearningAgentsActions::GetFloatAction(ParryAngle, 
           InActionObject, CombatActionObjectElement, Key_Action_Combat_Block,
           Settings->bVisLogEnabled, this, AgentId, AgentLocation);
		if (ensure(bSuccess))
			CombatActionsComponent->Block(ParryAngle);
	}
	else if (CombatActionName == Key_Action_Combat_Dodge)
	{
		FVector DodgeDirection = FVector::ZeroVector;
		bool bSuccess = ULearningAgentsActions::GetDirectionAction(DodgeDirection, 
           InActionObject, CombatActionObjectElement, AgentTransform, Key_Action_Combat_Dodge,
           Settings->bVisLogEnabled, this, AgentId, AgentLocation);
		if (ensure(bSuccess))
			CombatActionsComponent->Dodge(DodgeDirection);
	}
	else
	{
		ensure(false);
	}
}

void ULearningAgentsInteractor_Combat::SampleLocomotionAction(const ULearningAgentsActionObject* InActionObject,
                                                              int32 AgentId, AActor* AgentActor, ULearningAgentLocomotionActionsComponent* LocomotionActionsComponent,
                                                              const FLearningAgentsActionObjectElement& RootActionObjectElement)
{
	auto AgentLocation = AgentActor->GetActorLocation();
	auto AgentTransform  = AgentActor->GetTransform();
	auto Settings = GetDefault<UCombatLearningSettings>();
	
	FName LocomotionActionName;
	FLearningAgentsActionObjectElement RootLocomotionActionObjectElement;
	bool bGetActionSuccess = ULearningAgentsActions::GetExclusiveUnionAction(LocomotionActionName, RootLocomotionActionObjectElement,
		InActionObject,  RootActionObjectElement, Key_Action_Locomotion);
	if (!ensure(bGetActionSuccess))
		return;
	
	if (LocomotionActionName == Key_Action_Locomotion_Blocking)
	{
		uint8 LocomotionActionRaw = 0;
		bool bSuccess = ULearningAgentsActions::GetEnumAction(LocomotionActionRaw,
			InActionObject, RootLocomotionActionObjectElement, StaticEnum<ELALocomotionAction>(), Key_Action_Locomotion_Blocking,
			Settings->bVisLogEnabled, this, AgentId, AgentLocation);
		if (ensure(bSuccess))
		{
			auto LocomotionAction = static_cast<ELALocomotionAction>(LocomotionActionRaw);
			switch (LocomotionAction)
			{
				case ELALocomotionAction::Jump:
					LocomotionActionsComponent->Jump();
					break;
				case ELALocomotionAction::Mantle:
					LocomotionActionsComponent->Mantle();
					break;
				default:
					ensure(false);
					break;
			}
		}
	}
	else if (LocomotionActionName == Key_Action_Locomotion_NonBlocking)
	{
		TMap<FName, FLearningAgentsActionObjectElement> NonBlockingLocomotionActionObjectElements;
		bool bGotNonBlockingActions = ULearningAgentsActions::GetInclusiveUnionAction(NonBlockingLocomotionActionObjectElements, InActionObject,
			RootLocomotionActionObjectElement, Key_Action_Locomotion_NonBlocking);
		if (ensure(bGotNonBlockingActions && !NonBlockingLocomotionActionObjectElements.IsEmpty()))
		{
			SampleNonBlockingLocomotionActions(InActionObject, NonBlockingLocomotionActionObjectElements, LocomotionActionsComponent,
				AgentId, AgentLocation, AgentTransform, Settings);
		}
	}
	else
	{
		ensure(false);
	}
}

void ULearningAgentsInteractor_Combat::SampleNonBlockingLocomotionActions(const ULearningAgentsActionObject* InActionObject,
	const TMap<FName, FLearningAgentsActionObjectElement>& NonBlockingLocomotionActionObjectElements,
	ULearningAgentLocomotionActionsComponent* LocomotionActionsComponent, int32 AgentId,
	const FVector& AgentLocation, const FTransform& AgentTransform,
	const UCombatLearningSettings* Settings)
{
	if (auto SetSpeedAction = NonBlockingLocomotionActionObjectElements.Find(Key_Action_Locomotion_SetSpeed))
	{
		float SpeedNormalized = 0.f;
		bool bSuccess = ULearningAgentsActions::GetFloatAction(SpeedNormalized, InActionObject, *SetSpeedAction, Key_Action_Locomotion_SetSpeed,
		                                                       Settings->bVisLogEnabled, this, AgentId, AgentLocation);
		// TODO consider clamping low values of sampled value
		if (ensure(bSuccess))
			LocomotionActionsComponent->SetSpeed(SpeedNormalized * Settings->MaxSpeed);
	}
			
	if (auto MoveAction = NonBlockingLocomotionActionObjectElements.Find(Key_Action_Locomotion_Move))
	{
		FVector MoveDirection = FVector::ZeroVector;
		bool bSuccess = ULearningAgentsActions::GetDirectionAction(MoveDirection,  InActionObject, *MoveAction, AgentTransform, 
			Key_Action_Locomotion_Move, 
           Settings->bVisLogEnabled, this, AgentId);
		if (ensure(bSuccess))
			LocomotionActionsComponent->AddMoveInput(MoveDirection);
	}
			
	if (auto RotateAction = NonBlockingLocomotionActionObjectElements.Find(Key_Action_Locomotion_Rotate))
	{
		FRotator Rotator;
		bool bSuccess = ULearningAgentsActions::GetRotationAction(Rotator, 
			InActionObject, *RotateAction,AgentTransform.GetRotation().Rotator(), Key_Action_Locomotion_Rotate,
			Settings->bVisLogEnabled, this, AgentId, AgentLocation);
		if (ensure(bSuccess))
			LocomotionActionsComponent->AddRotationInput(Rotator);
	}
			
	if (auto SayPhraseAction = NonBlockingLocomotionActionObjectElements.Find(Key_Action_SayPhrase))
	{
		FName PhraseName;
		bool bSuccess = ULearningAgentsActions::GetNamedExclusiveDiscreteAction(PhraseName, InActionObject, *SayPhraseAction, Key_Action_SayPhrase,
			Settings->bVisLogEnabled, this, AgentId, AgentLocation);
		if (ensure(bSuccess))
		{
			FGameplayTag PhraseTag = FGameplayTag::RequestGameplayTag(PhraseName);
			LocomotionActionsComponent->SayPhrase(PhraseTag);
		}
	}
			
	if (auto AnimationAction = NonBlockingLocomotionActionObjectElements.Find(Key_Action_Locomotion_NonBlocking_Animation))
	{
		FName AnimationActionName; 
		FLearningAgentsActionObjectElement AnimationActionObjectElement;
		bool bGotAnimationAction = ULearningAgentsActions::GetExclusiveUnionAction(AnimationActionName, AnimationActionObjectElement,
			InActionObject,*AnimationAction, Key_Action_Locomotion_NonBlocking_Animation);
				
		if (ensure(bGotAnimationAction))
		{
			SampleLocomotionAnimationAction(InActionObject, AnimationActionName, AnimationActionObjectElement,
			                                LocomotionActionsComponent, AgentId, AgentLocation, Settings);	
		}
	}
}

void ULearningAgentsInteractor_Combat::SampleLocomotionAnimationAction(const ULearningAgentsActionObject* InActionObject,
   const FName& AnimationActionName, const FLearningAgentsActionObjectElement& AnimationActionObjectElement,
   ULearningAgentLocomotionActionsComponent* LocomotionActionsComponent, int32 AgentId,
   const FVector& AgentLocation, const UCombatLearningSettings* Settings)
{
	if (AnimationActionName == Key_Action_Gesture)
	{
		FName GestureName;
		bool bSuccess = ULearningAgentsActions::GetNamedExclusiveDiscreteAction(GestureName, InActionObject,
			AnimationActionObjectElement, Key_Action_Gesture,
			Settings->bVisLogEnabled, this, AgentId, AgentLocation);
		if (ensure(bSuccess))
		{
			FGameplayTag GestureTag = FGameplayTag::RequestGameplayTag(GestureName);
			LocomotionActionsComponent->Gesture(GestureTag);
		}
	}
	else if (AnimationActionName == Key_Action_UseConsumableItem)
	{
		FName ItemIdName;
		bool bSuccess = ULearningAgentsActions::GetNamedExclusiveDiscreteAction(ItemIdName,
			InActionObject, AnimationActionObjectElement, Key_Action_UseConsumableItem,
			Settings->bVisLogEnabled, this, AgentId, AgentLocation);
		if (ensure(bSuccess))
		{
			FGameplayTag ItemId = FGameplayTag::RequestGameplayTag(ItemIdName);
			LocomotionActionsComponent->UseItem(ItemId);
		}
	}
	else if (AnimationActionName == Key_Action_ChangeWeaponState)
	{
		uint8 WeaponStateChangeRaw = 0;
		bool bSuccess = ULearningAgentsActions::GetEnumAction(WeaponStateChangeRaw,
			InActionObject, AnimationActionObjectElement, StaticEnum<ELAWeaponStateChange>(), Key_Action_ChangeWeaponState,
			Settings->bVisLogEnabled, this, AgentId, AgentLocation);
		if (ensure(bSuccess))
		{
			ELAWeaponStateChange WeaponStateChange = static_cast<ELAWeaponStateChange>(WeaponStateChangeRaw);
			LocomotionActionsComponent->SetWeaponReady(WeaponStateChange == ELAWeaponStateChange::Ready);
		}
	}
	else
	{
		ensure(false);
	}
}

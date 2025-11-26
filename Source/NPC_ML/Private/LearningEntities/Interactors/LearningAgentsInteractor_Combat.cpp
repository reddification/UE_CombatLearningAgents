// 
#include "LearningEntities/Interactors/LearningAgentsInteractor_Combat.h"

#include "Components/LearningAgentCombatObservationComponent.h"
#include "Data/LearningAgentsDataTypes.h"
#include "Data/LearningAgentsTags_Combat.h"
#include "Settings/CombatLearningSettings.h"

namespace LAObservationKeys
{
	FName Key_Observation_Health = FName("Observation.Status.Self.Health");
	FName Key_CombatObservations = FName("Observation.Combat.All");
	FName Key_Observation_Self = FName("Observation.Self");

	FName Key_Observation_Surrounding = FName("Observation.Surrounding");
	FName Key_Observation_Surrounding_LIDAR = FName("Observation.Surrounding.LIDAR");
	FName Key_Observation_Surrounding_LIDAR_Ceiling = FName("Observation.Surrounding.LIDAR.Ceiling");
	FName Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Item = FName("Observation.Surrounding.LIDAR.Raindrop.Downward.Item");
	FName Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Array = FName("Observation.Surrounding.LIDAR.Raindrop.Downward.Array");
	FName Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Convolved = FName("Observation.Surrounding.LIDAR.Raindrop.Downward.Convolved");
	FName Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Item = FName("Observation.Surrounding.LIDAR.Raindrop.Forward.Item");
	FName Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Array = FName("Observation.Surrounding.LIDAR.Raindrop.Forward.Array");
	FName Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Convolved = FName("Observation.Surrounding.LIDAR.Raindrop.Forward.Convolved");
	FName Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Item = FName("Observation.Surrounding.LIDAR.Raindrop.Backward.Item");
	FName Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Array = FName("Observation.Surrounding.LIDAR.Raindrop.Backward.Array");
	FName Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Convolved = FName("Observation.Surrounding.LIDAR.Raindrop.Backward.Convolved");

	FName Key_Observation_Allies = FName("Observation.Allies");
	FName Key_Observation_Environment = FName("Observation.Environment");

	FName Key_Observation_Self_Dynamic = FName("Observation.Self.Dynamic");
	FName Key_Observation_Self_Dynamic_Health = FName("Observation.Self.Dynamic.Health");
	FName Key_Observation_Self_Dynamic_Stamina = FName("Observation.Self.Dynamic.Stamina");
	FName Key_Observation_Self_Dynamic_Velocity = FName("Observation.Self.Dynamic.Velocity");
	FName Key_Observation_Self_Dynamic_CombatStates = FName("Observation.Self.Dynamic.CombatStates");
	FName Key_Observation_Self_Dynamic_Gesture = FName("Observation.Self.Dynamic.Gesture.");
	FName Key_Observation_Self_Dynamic_Gesture_Optional = FName("Observation.Self.Dynamic.Gesture.Optional");

	FName Key_Observation_Self_Dynamic_ActiveWeapon = FName("Observation.Self.Dynamic.ActiveWeapon.Range");
	FName Key_Observation_Self_Dynamic_ActiveWeapon_Range = FName("Observation.Self.Dynamic.ActiveWeapon.Range");
	FName Key_Observation_Self_Dynamic_ActiveWeapon_Range_Melee = FName("Observation.Self.Dynamic.ActiveWeapon.Range.Melee");
	FName Key_Observation_Self_Dynamic_ActiveWeapon_Range_Ranged = FName("Observation.Self.Dynamic.ActiveWeapon.Range.Ranged");
	FName Key_Observation_Self_Dynamic_ActiveWeapon_Type = FName("Observation.Self.Dynamic.ActiveWeapon.Type");
	FName Key_Observation_Self_Dynamic_ActiveWeapon_Mastery = FName("Observation.Self.Dynamic.ActiveWeapon.Mastery");
	FName Key_Observation_Self_Dynamic_ActiveWeapon_PowerRate = FName("Observation.Self.Dynamic.ActiveWeapon.PowerRate");

	FName Key_Observation_Self_Static = FName("Observation.Self.Static");
	FName Key_Observation_Self_Static_Level = FName("Observation.Self.Static.Level");
	FName Key_Observation_Self_Static_ArmorRate = FName("Observation.Self.Static.ArmorRate");
	FName Key_Observation_Self_Static_SurvivalDesire = FName("Observation.Self.Static.SurvivalDesire");


	FName Key_Observation_Enemies = FName("Observation.Enemy.Array");
	FName Key_Observation_Enemy = FName("Observation.Enemy");
	FName Key_Observation_Enemy_Dynamic = FName("Observation.Enemy.Dynamic");
	FName Key_Observation_Enemy_Dynamic_Observations = FName("Observation.Enemy.Dynamic.Observations");
	FName Key_Observation_Enemy_Static = FName("Observation.Enemy.Static");
	FName Key_Observation_Enemy_Static_Observations = FName("Observation.Enemy.Static.Observations");

	FName Key_Observation_Enemy_Dynamic_Health = FName("Observation.Enemy.Dynamic.Health");
	FName Key_Observation_Enemy_Dynamic_IsAlive = FName("Observation.Enemy.Dynamic.IsAlive");
	FName Key_Observation_Enemy_Dynamic_Velocity = FName("Observation.Enemy.Dynamic.Velocity");
	FName Key_Observation_Enemy_Dynamic_Location = FName("Observation.Enemy.Dynamic.Location");
	FName Key_Observation_Enemy_Dynamic_Orientation = FName("Observation.Enemy.Dynamic.Orientation");
	FName Key_Observation_Enemy_Dynamic_AgentCanSeeEnemy = FName("Observation.Enemy.Dynamic.AgentCanSeeEnemy");
	FName Key_Observation_Enemy_Dynamic_EnemyCanSeeAgent = FName("Observation.Enemy.Dynamic.EnemyCanSeeAgent");
	FName Key_Observation_Enemy_Dynamic_KillDesire = FName("Observation.Enemy.Dynamic.KillDesire");
	FName Key_Observation_Enemy_Dynamic_Gesture = FName("Observation.Enemy.Dynamic.Gesture");
	FName Key_Observation_Enemy_Dynamic_Gesture_Optional = FName("Observation.Enemy.Dynamic.Gesture.Optional");
	FName Key_Observation_Enemy_Dynamic_CombatState = FName("Observation.Enemy.Dynamic.CombatState");
	FName Key_Observation_Enemy_Dynamic_Weapon = FName("Observation.Enemy.Dynamic.Weapon");

	FName Key_Observation_Enemy_Static_ArmorRate = FName("Observation.Enemy.Static.ArmorRate");
	FName Key_Observation_Enemy_Static_Level = FName("Observation.Enemy.Static.Level");

	FName Key_Observation_Environment_TotalCombatDuration = FName("Observation.Environment.TotalCombatDuration");

	FName Key_Observation_Ally = FName("Observation.Ally");
	FName Key_Observation_Ally_Dynamic = FName("Observation.Ally.Dynamic");
	FName Key_Observation_Ally_Dynamic_IsAlive = FName("Observation.Ally.Dynamic.IsAlive");
	FName Key_Observation_Ally_Dynamic_Health = FName("Observation.Ally.Dynamic.Health");
	FName Key_Observation_Ally_Dynamic_Velocity = FName("Observation.Ally.Dynamic.Velocity");
	FName Key_Observation_Ally_Dynamic_Location = FName("Observation.Ally.Dynamic.Location");
	FName Key_Observation_Ally_Dynamic_Orientation = FName("Observation.Ally.Dynamic.Orientation");
	FName Key_Observation_Ally_Dynamic_Gesture = FName("Observation.Ally.Dynamic.Gesture");
	FName Key_Observation_Ally_Dynamic_Gesture_Optional = FName("Observation.Ally.Dynamic.Optional");
	FName Key_Observation_Ally_Dynamic_CombatState = FName("Observation.Ally.Dynamic.CombatState");
	FName Key_Observation_Ally_Dynamic_AgentCanSeeAlly = FName("Observation.Ally.Dynamic.AgentCanSeeAlly");
	FName Key_Observation_Ally_Dynamic_Weapon = FName("Observation.Ally.Dynamic.Weapon");
	FName Key_Observation_Ally_Dynamic_Observations = FName("Observation.Ally.Dynamic.Observations");
	FName Key_Observation_Ally_Static = FName("Observation.Ally.Static");
	FName Key_Observation_Ally_Static_Observations = FName("Observation.Ally.Static.Observations");
	FName Key_Observation_Ally_Static_Level = FName("Observation.Ally.Static.Level");
	FName Key_Observation_Ally_Static_ArmorRate = FName("Observation.Ally.Static.ArmorRate");
}

namespace LAActionKeys
{
	FName Key_Action_All = FName("Action.All");
	
	FName Key_Action_Locomotion_SetSpeed = FName("Action.Locomotion.SetSpeed");
	FName Key_Action_Locomotion_Move = FName("Action.Locomotion.Move");
	FName Key_Action_Locomotion_Rotate = FName("Action.Locomotion.Rotate");
	FName Key_Action_Locomotion_Jump = FName("Action.Locomotion.Jump");
	FName Key_Action_Locomotion_Climb = FName("Action.Locomotion.Climb");
	FName Key_Action_Locomotion_Null = FName("Action.Locomotion.Null");
	FName Key_Action_Locomotion_NonBlocking = FName("Action.Locomotion.NonBlocking");
	FName Key_Actions_Locomotion = FName("Action.Locomotion");
	
	// these are like "actions that can be done inclusively with locomotion, but not locomotion themselves"
	FName Key_Action_Gesture = FName("Action.Gesture");
	FName Key_Action_SayPhrase = FName("Action.SayPhrase");
	FName Key_Action_UseConsumableItem = FName("Action.UseConsumableItem");
	
	FName Key_Action_Combat = FName("Action.Combat");
	FName Key_Action_Combat_Attack = FName("Action.Combat.Attack");
	FName Key_Action_Combat_Parry = FName("Action.Combat.Parry");
	FName Key_Action_Combat_Dodge = FName("Action.Combat.Dodge");
	FName Key_Action_Combat_Null = FName("Action.Combat.Null");
	
	FName Key_Action_Null = FName("Action.Null");
}

using namespace LAActionKeys;
using namespace LAObservationKeys;

/*
 * Combat observation hierarchy
 * All
 *		Self
 *			Dynamic
 *				fHealth
 *				fStamina
 *				vVelocity
 *				eCombatState (none, attacking, dodging, staggered, blocking, parried attack, sprinting, sneaking)
 *				tWeapon
 *					fAttackRange
 *					fWeaponMastery
 *					eWeaponType (unarmed, melee, ranged)
 *					fPowerRate
 *			Static	
 *				iLevel
 *				fArmorRate
 *				fSurviveDesire
 *			
 *		LIDAR
 *			Raindrops (heightmaps in several planes. alternative would be having a tensor with a fragment of octree surrounding an agent, but there's no conv3d yet)
 *				Downward
 *				Forward
*				Backward
 *			
 *		Enemies
 *			Dynamic
 *				bAlive
 *				fHealth
 *				vVelocity
 *				vRelativeOrientation
 *				vRelativeLocation (can be both "known" and "assumed". I hope that "bCanSee" observation can help to make the difference for the LA)
 *				bAgentCanSeeEnemy
 *				bAgentCanBeSeenByEnemy
 *				fKillDesire
*				eActiveGesture
*				bReachable				
*				eCombatState (none, attacking, dodging, staggered, blocking, parried attack, sprinting, sneaking)
*				tWeapon
*					fAttackRange
*					fWeaponMastery
*					eWeaponType (unarmed, melee, ranged)
*					fPowerRate (assumed)
*			Static
*				fArmorRate (assumed)
*				
 *		Allies
 *			Dynamic
 *				bAlive
 *				fHealth
 *				vRelativeVelocity
 *				vRelativeOrientation
 *				vRelativeLocation
*				eActiveGesture
*				eActivePhrase
*				bInCombat
*				eCombatState (none, attacking, dodging, staggered, blocking, parried attack)
*				tWeapon
*					fAttackRange
*					fWeaponMastery
*					eWeaponType (unarmed, melee, ranged)
*					fPowerRate
*			Static
*				fArmorRate
*		Neutrals (just neutral characters, think of citizens, non-aggressive animals, bypassers)
*			Dynamic
*			//... same as allies
*			Static
*			//... same as allies
*			
*		Environment
*			fCombatTotalDuration
 */

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
		GetSelfObservationsSchema(InObservationSchema, Settings), Key_Observation_Self);
		Observations.Add(Key_Observation_Self, SelfObservations);
	}
	
	if (Settings->RelevantObservations.HasTag(LearningAgentsTags_Combat::Observation_Surrounding))
	{
		auto LidarObservations = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema,
		GetLidarObservationsSchema(InObservationSchema, Settings), Key_Observation_Surrounding_LIDAR);
		Observations.Add(Key_Observation_Surrounding_LIDAR, LidarObservations);
	}

	if (Settings->RelevantObservations.HasTag(LearningAgentsTags_Combat::Observation_Enemy))
	{
		auto EnemiesObservations = ULearningAgentsObservations::SpecifyArrayObservation(InObservationSchema,
			GetEnemyObservationSchema(InObservationSchema, Settings),
			Settings->MaxEnemiesObservedAtOnce, 128, 4, 128, Key_Observation_Enemies);
		Observations.Add(Key_Observation_Enemies, EnemiesObservations);
	}

	if (Settings->RelevantObservations.HasTag(LearningAgentsTags_Combat::Observation_Ally))
	{
		auto AlliesObservations = ULearningAgentsObservations::SpecifyArrayObservation(InObservationSchema,
	   GetAllyObservationsSchema(InObservationSchema, Settings),
			Settings->MaxAlliesObservedAtOnce, 128, 4, 64, Key_Observation_Allies);
		Observations.Add(Key_Observation_Allies, AlliesObservations);
	}

	if (Settings->RelevantObservations.HasTag(LearningAgentsTags_Combat::Observation_CombatState))
	{
		auto EnvironmentObservations = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema,
			GetCombatStateObservationSchema(InObservationSchema), Key_Observation_Environment);
		Observations.Add(Key_Observation_Environment, EnvironmentObservations);
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
	
	auto SetSpeedAction = ULearningAgentsActions::SpecifyFloatAction(InActionSchema, Settings->MaxSpeed, Key_Action_Locomotion_SetSpeed);
	auto MoveAction = ULearningAgentsActions::SpecifyDirectionAction(InActionSchema, Key_Action_Locomotion_Move);
	auto RotateAction = ULearningAgentsActions::SpecifyRotationAction(InActionSchema, Settings->RotationScale, Key_Action_Locomotion_Rotate);
	auto JumpAction = ULearningAgentsActions::SpecifyLocationAction(InActionSchema, Settings->MaxJumpDistance, Key_Action_Locomotion_Jump);
	// auto JumpActionOptional = ULearningAgentsActions::SpecifyOptionalAction(InActionSchema, JumpAction, 0.2f, Key_Action_Jump_Optional);
	
	auto ClimbAction = ULearningAgentsActions::SpecifyDirectionAction(InActionSchema, Key_Action_Locomotion_Climb);

	auto DodgeAction = ULearningAgentsActions::SpecifyDirectionAction(InActionSchema, Key_Action_Combat_Dodge);
	// auto DodgeActionOptional = ULearningAgentsActions::SpecifyOptionalAction(InActionSchema, DodgeAction, 0.1f, Key_Action_Dodge_Optional);
	
	auto ParryAction = ULearningAgentsActions::SpecifyDirectionAction(InActionSchema, Key_Action_Combat_Parry);
	// auto ParryActionOptional = ULearningAgentsActions::SpecifyOptionalAction(InActionSchema, ParryAction, 0.2f, Key_Action_Parry_Optional);
	
	auto AttackAction = ULearningAgentsActions::SpecifyExclusiveDiscreteAction(InActionSchema, 
		Settings->AttackActionsProbabilities.Num(), Settings->AttackActionsProbabilities,
		Key_Action_Combat_Attack);

	bool bUseConsumableItemActionValid = false;
	FLearningAgentsActionSchemaElement UseConsumableItemAction;
	if (!Settings->ConsumablesProbabilities.IsEmpty())
	{
		UseConsumableItemAction = ULearningAgentsActions::SpecifyExclusiveDiscreteAction(InActionSchema, 
			Settings->ConsumablesProbabilities.Num(), Settings->ConsumablesProbabilities, 
			Key_Action_UseConsumableItem);
		bUseConsumableItemActionValid = true;
	}
	
	FLearningAgentsActionSchemaElement GestureAction;
	bool bGestureActionValid = false;
	if (!Settings->Gestures.IsEmpty())
	{
		GestureAction = GetNamedOptionsAction(InActionSchema, Settings->Gestures, Key_Action_Gesture);
		bGestureActionValid = true;
	}
	
	FLearningAgentsActionSchemaElement SayPhraseAction;
	bool bSayPhraseActionValid = false;
	if (!Settings->Phrases.IsEmpty())
	{
		SayPhraseAction = GetNamedOptionsAction(InActionSchema, Settings->Phrases, Key_Action_SayPhrase);
		bSayPhraseActionValid = true;
	}
	
	auto NullLocomotionAction = ULearningAgentsActions::SpecifyNullAction(InActionSchema, Key_Action_Locomotion_Null);
	TMap<FName, FLearningAgentsActionSchemaElement> NonBlockingLocomotionActions = 
	{
		{ Key_Action_Locomotion_SetSpeed, SetSpeedAction },
		{ Key_Action_Locomotion_Move, MoveAction },
		{ Key_Action_Locomotion_Rotate, RotateAction },
		{ Key_Action_Locomotion_Null, NullLocomotionAction }
	};
	
	// i'm not sure if probabilities should sum up to exactly 1 or not 
	TMap<FName, float> NonBlockingLocomotionActionsProbabilities = 
	{
		{ Key_Action_Locomotion_SetSpeed, 0.2f },
		{ Key_Action_Locomotion_Move, 0.6f, },
		{ Key_Action_Locomotion_Rotate, 0.4f },
		{ Key_Action_Locomotion_Null, 0.5f }
	};
	
	// idk, i should either come up with a proper name for inclusive actions when just walking
	// or make a separate inclusive action for actions while walking and have true locomotion actions be their own inclusive action
	// TODO think about it
	if (bGestureActionValid)
	{
		NonBlockingLocomotionActions.Emplace(Key_Action_Gesture, GestureAction);
		NonBlockingLocomotionActionsProbabilities.Emplace(Key_Action_Gesture, 0.2f);
	}

	if (bSayPhraseActionValid)
	{
		NonBlockingLocomotionActions.Emplace(Key_Action_SayPhrase, SayPhraseAction);
		NonBlockingLocomotionActionsProbabilities.Emplace(Key_Action_SayPhrase, 0.2f);
	}
	
	if (bUseConsumableItemActionValid)
	{
		NonBlockingLocomotionActions.Emplace(Key_Action_UseConsumableItem, UseConsumableItemAction);
		NonBlockingLocomotionActionsProbabilities.Emplace(Key_Action_UseConsumableItem, 0.15f);
	}
	
	auto NonBlockingLocomotionActionsUnion = ULearningAgentsActions::SpecifyInclusiveUnionAction(InActionSchema, 
		NonBlockingLocomotionActions, NonBlockingLocomotionActionsProbabilities,
		Key_Action_Locomotion_NonBlocking);
	
	TMap<FName, FLearningAgentsActionSchemaElement> ExclusiveLocomotionActionsMap = 
	{
		{ Key_Action_Locomotion_Jump, JumpAction },
		{ Key_Action_Locomotion_Climb, ClimbAction },
		{ Key_Action_Locomotion_NonBlocking, NonBlockingLocomotionActionsUnion },
		{ Key_Action_Locomotion_Null, NullLocomotionAction }
	};
	
	TMap<FName, float> ExclusiveLocomotionActionsProbabilities = 
	{
		{ Key_Action_Locomotion_Jump, 0.1f, },
		{ Key_Action_Locomotion_Climb, 0.1f, },
		{ Key_Action_Locomotion_NonBlocking, 0.6f },
		{ Key_Action_Locomotion_Null, 0.2f }
	};
	
	auto ExclusiveLocomotionActionsUnion = ULearningAgentsActions::SpecifyExclusiveUnionAction(InActionSchema,
		ExclusiveLocomotionActionsMap, ExclusiveLocomotionActionsProbabilities, Key_Actions_Locomotion);
	
	auto CombatNullAction = ULearningAgentsActions::SpecifyNullAction(InActionSchema, Key_Action_Combat_Null);
	TMap<FName, FLearningAgentsActionSchemaElement> CombatActions = 
	{
		{ Key_Action_Combat_Attack, AttackAction },
		{ Key_Action_Combat_Parry, ParryAction },
		{ Key_Action_Combat_Dodge, DodgeAction },
		{ Key_Action_Combat_Null, CombatNullAction }
	};
	
	TMap<FName, float> CombatActionsProbabilities = 
	{
		{ Key_Action_Combat_Attack, 0.6f },
		{ Key_Action_Combat_Parry, 0.3f },
		{ Key_Action_Combat_Dodge, 0.2f },
		{ Key_Action_Combat_Null, 0.2f }
	};
	
	auto CombatActionsExclusiveUnion = ULearningAgentsActions::SpecifyExclusiveUnionAction(InActionSchema, CombatActions, CombatActionsProbabilities,
		Key_Action_Combat);
	
	auto NullAction = ULearningAgentsActions::SpecifyNullAction(InActionSchema, Key_Action_Null);
	TMap<FName, FLearningAgentsActionSchemaElement> AllActions = 
	{
		{ Key_Actions_Locomotion, ExclusiveLocomotionActionsUnion },
		{ Key_Action_Combat, CombatActionsExclusiveUnion },
		{ Key_Action_Null, NullAction }
	};
	
	TMap<FName, float> AllActionsProbabilities = 
	{
		{ Key_Actions_Locomotion, 0.6f },
		{ Key_Action_Combat, 0.3f },
		{ Key_Action_Null, 0.1f }
	};
	
	OutActionSchemaElement = ULearningAgentsActions::SpecifyExclusiveUnionAction(InActionSchema, AllActions, AllActionsProbabilities, Key_Action_All);
}

FLearningAgentsActionSchemaElement ULearningAgentsInteractor_Combat::GetNamedOptionsAction(ULearningAgentsActionSchema* InActionSchema,
	const FGameplayTagContainer& OptionTags, const FName& ActionTag) const
{
	TArray<FName> OptionNames;
	TMap<FName, float> OptionProbabilities;
	OptionNames.Reserve(OptionTags.Num());
	OptionProbabilities.Reserve(OptionTags.Num());
		
	// idk, maybe users should be able to provide their own base probabilities
	const float UniformProbability = 1.f / OptionTags.Num();
	for (const auto& Option : OptionTags)
	{
		auto OptionName = Option.GetTagName();
			
		OptionNames.Emplace(OptionName);
		OptionProbabilities.Emplace(OptionName, UniformProbability);
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
	if (AllMaskedActions.Contains(Key_Actions_Locomotion))
		MaskedTopLevelActions.Emplace(Key_Actions_Locomotion);
	
	if (AllMaskedActions.Contains(Key_Action_Combat))
		MaskedTopLevelActions.Emplace(Key_Action_Combat);
	
	// =========================== combat actions modifiers
	
	// i don't really know if I should provide any modifiers for attack, parry and dodge actions. 
	// it's like there isn't anything meaningful I could specify for them
	auto AttackActionModifier = ULearningAgentsActions::MakeExclusiveDiscreteActionModifier(InActionModifier, {}, Key_Action_Combat_Attack);
	auto ParryActionModifier = ULearningAgentsActions::MakeDirectionActionModifier(InActionModifier,
		FVector::ZeroVector, false, false, true, FTransform::Identity,
		Key_Action_Combat_Parry);
	auto DodgeActionModifier = ULearningAgentsActions::MakeDirectionActionModifier(InActionModifier, 
		FVector::ZeroVector, false, false, true, FTransform::Identity,
		Key_Action_Combat_Dodge);
	
	TMap<FName, FLearningAgentsActionModifierElement> CombatActionModifiersMap = 
	{
		{ Key_Action_Combat_Attack, AttackActionModifier },
		{ Key_Action_Combat_Parry, ParryActionModifier },
		{ Key_Action_Combat_Dodge, DodgeActionModifier },
		{ Key_Action_Combat_Null, ULearningAgentsActions::MakeNullActionModifier(InActionModifier, Key_Action_Combat_Null) }
	};
	
	TSet<FName> AllCombatActions = { Key_Action_Combat_Attack, Key_Action_Combat_Parry, Key_Action_Combat_Dodge };
	TArray<FName> MaskedCombatActions = AllMaskedActions.Intersect(AllCombatActions).Array();
	auto CombatActionsModifier = ULearningAgentsActions::MakeExclusiveUnionActionModifier(InActionModifier, CombatActionModifiersMap, MaskedCombatActions);

	// =================================== non blocking locomotion actions modifiers
	
	const bool bHasGestureAction = !Settings->Gestures.IsEmpty();
	const bool bHasSayPhraseAction = !Settings->Phrases.IsEmpty();
	const bool bHasUseConsumableItemAction = !Settings->ConsumablesProbabilities.IsEmpty();
	
	TSet<FName> AllNonBlockingLocomotionActions = { Key_Action_Locomotion_SetSpeed, Key_Action_Locomotion_Move, Key_Action_Locomotion_Rotate };
	if (bHasGestureAction)
		AllNonBlockingLocomotionActions.Emplace(Key_Action_Gesture);
	
	if (bHasSayPhraseAction)
		AllNonBlockingLocomotionActions.Emplace(Key_Action_SayPhrase);
	
	if (bHasUseConsumableItemAction)
		AllNonBlockingLocomotionActions.Emplace(Key_Action_UseConsumableItem);
	
	TArray<FName> MaskedNonBlockingLocomotionActions = AllMaskedActions.Intersect(AllNonBlockingLocomotionActions).Array();
	
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
	TMap<FName, FLearningAgentsActionModifierElement> NonBlockingLocomotionActionModifiersMap = 
	{
		{ Key_Action_Locomotion_SetSpeed, SetSpeedActionModifier },
		{ Key_Action_Locomotion_Move, MoveActionModifier },
		{ Key_Action_Locomotion_Rotate, RotateActionModifier },
		{ Key_Action_Locomotion_Null, ULearningAgentsActions::MakeNullActionModifier(InActionModifier, Key_Action_Locomotion_Null) }
	};
	
	if (bHasGestureAction)
	{
		auto GestureActionModifier = ULearningAgentsActions::MakeNamedExclusiveDiscreteActionModifier(InActionModifier, {}, Key_Action_Gesture);
		NonBlockingLocomotionActionModifiersMap.Emplace(Key_Action_Gesture, GestureActionModifier);
	}
	
	if (bHasSayPhraseAction)
	{
		auto SayPhraseActionModifier = ULearningAgentsActions::MakeNamedExclusiveDiscreteActionModifier(InActionModifier, {}, Key_Action_SayPhrase);
		NonBlockingLocomotionActionModifiersMap.Emplace(Key_Action_SayPhrase, SayPhraseActionModifier);
	}
	
	if (bHasUseConsumableItemAction)
	{
		auto UseConsumableItemModifier = ULearningAgentsActions::MakeExclusiveDiscreteActionModifier(InActionModifier, {}, Key_Action_UseConsumableItem);
		NonBlockingLocomotionActionModifiersMap.Emplace(Key_Action_UseConsumableItem, UseConsumableItemModifier);
	}
	
	auto NonBlockingLocomotionUnionActionModifier = ULearningAgentsActions::MakeInclusiveUnionActionModifier(InActionModifier, 
		NonBlockingLocomotionActionModifiersMap, MaskedNonBlockingLocomotionActions, Key_Action_Locomotion_NonBlocking);
	
	// ===================================
	auto JumpActionModifier = ULearningAgentsActions::MakeLocationActionModifier(InActionModifier, 
		FVector::ZeroVector, false, false, false, FTransform::Identity,
		Key_Action_Locomotion_Jump);
	auto ClimbActionModifier = ULearningAgentsActions::MakeDirectionActionModifier(InActionModifier, 
		FVector::ZeroVector, false, false, false, FTransform::Identity,
		Key_Action_Locomotion_Climb);
	
	TMap<FName, FLearningAgentsActionModifierElement> LocomotionActionModifiersMap = 
	{
		{ Key_Action_Locomotion_Jump, JumpActionModifier },
		{ Key_Action_Locomotion_Climb, ClimbActionModifier },
		{ Key_Action_Locomotion_NonBlocking, NonBlockingLocomotionUnionActionModifier },
		{ Key_Action_Locomotion_Null, ULearningAgentsActions::MakeNullActionModifier(InActionModifier, Key_Action_Locomotion_Null ) }
	};
	
	TSet<FName> AllLocomotionActions = { Key_Action_Locomotion_Jump, Key_Action_Locomotion_Climb, Key_Action_Locomotion_NonBlocking };
	TArray<FName> MaskedLocomotionActions = AllMaskedActions.Intersect(AllLocomotionActions).Array();
	
	auto LocomotionActionsModifier = ULearningAgentsActions::MakeExclusiveUnionActionModifier(InActionModifier, LocomotionActionModifiersMap,
		MaskedLocomotionActions);
	
	auto RootLevelNullActionModifier = ULearningAgentsActions::MakeNullActionModifier(InActionModifier, Key_Action_Null);
	TMap<FName, FLearningAgentsActionModifierElement> RootActionModifiers = 
	{
		{ Key_Actions_Locomotion, LocomotionActionsModifier },
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
		Observations.Add(Key_Observation_Environment, CombatStateObservation);
	}
	
	if (Settings->RelevantObservations.HasTag(LearningAgentsTags_Combat::Observation_Enemy))
	{
		auto EnemiesData = CombatObservationComponent->GetEnemies();
		auto EnemiesObservations = GatherEnemiesObservation(InObservationObject, AgentId, EnemiesData);
		Observations.Add(Key_Observation_Enemies, EnemiesObservations);
	}

	if (Settings->RelevantObservations.HasTag(LearningAgentsTags_Combat::Observation_Ally))
	{
		auto AlliesData = CombatObservationComponent->GetAllies();
		auto AlliesObservation = GatherAlliesObservations(InObservationObject, AgentId, AlliesData);
		Observations.Add(Key_Observation_Allies, AlliesObservation);
	}

	OutObservationObjectElement = ULearningAgentsObservations::MakeStructObservation(InObservationObject, Observations, Key_CombatObservations);
}

void ULearningAgentsInteractor_Combat::PerformAgentAction_Implementation(
	const ULearningAgentsActionObject* InActionObject, const FLearningAgentsActionObjectElement& InActionObjectElement,
	const int32 AgentId)
{
	Super::PerformAgentAction_Implementation(InActionObject, InActionObjectElement, AgentId);
	// auto WalkOrSprintAction = ULearningAgentsActions::GetExc
}

FLearningAgentsObservationSchemaElement ULearningAgentsInteractor_Combat::GetWeaponObservationSchema(
	ULearningAgentsObservationSchema* InObservationSchema,
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
		StaticEnum<ELAActiveWeaponType>(), Key_Observation_Self_Dynamic_ActiveWeapon_Type);
	auto WeaponMasteryObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema,
		1.f, Key_Observation_Self_Dynamic_ActiveWeapon_Mastery);
	auto MeleeAttackRangeObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema,
		LearningSettings->MaxMeleeAttackRange, Key_Observation_Self_Dynamic_ActiveWeapon_Range_Melee);
	auto RangedAttackRangeObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema,
		LearningSettings->MaxRangedAttackRange, Key_Observation_Self_Dynamic_ActiveWeapon_Range_Ranged);
	auto AttackRangeObservation = ULearningAgentsObservations::SpecifyEitherObservation(InObservationSchema, MeleeAttackRangeObservation,
		RangedAttackRangeObservation, FloatObservationEncodingSize, Key_Observation_Self_Dynamic_ActiveWeapon_Range);
	auto PowerRateObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema,
		LearningSettings->MaxPowerRate, Key_Observation_Self_Dynamic_ActiveWeapon_PowerRate);
	
	TMap<FName, FLearningAgentsObservationSchemaElement> ActiveWeaponObservations =
	{
		{ Key_Observation_Self_Dynamic_ActiveWeapon_Type, ActiveWeaponTypeObservation },
		{ Key_Observation_Self_Dynamic_ActiveWeapon_Mastery, WeaponMasteryObservation },
		{ Key_Observation_Self_Dynamic_ActiveWeapon_Range, AttackRangeObservation},
		{ Key_Observation_Self_Dynamic_ActiveWeapon_PowerRate, PowerRateObservation }
	};
	
	return ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, ActiveWeaponObservations, Key_Observation_Self_Dynamic_ActiveWeapon);
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GetWeaponObservation(
	ULearningAgentsObservationObject* AgentObject, int AgentId,
	const FWeaponData& WeaponData, const UCombatLearningSettings* Settings, const FVector& AgentLocation) const
{
	auto MutableThis = const_cast<ULearningAgentsInteractor_Combat*>(this);
	
	auto ActiveWeaponTypeObservation = ULearningAgentsObservations::MakeEnumObservation(AgentObject, StaticEnum<ELAActiveWeaponType>(),
		static_cast<uint8>(WeaponData.WeaponType), Key_Observation_Self_Dynamic_ActiveWeapon_Type,
		Settings->bVisLogEnabled, MutableThis, AgentId, AgentLocation);
	
	auto WeaponMasteryObservation = ULearningAgentsObservations::MakeFloatObservation(AgentObject, WeaponData.NormalizedWeaponMastery,
		Key_Observation_Self_Dynamic_ActiveWeapon_Mastery,
		Settings->bVisLogEnabled, MutableThis, AgentId, AgentLocation);

	FLearningAgentsObservationObjectElement AttackRangeObservation;
	if (WeaponData.WeaponType == ELAActiveWeaponType::Ranged)
	{
		auto RangedAttackRangeObservation = ULearningAgentsObservations::MakeFloatObservation(AgentObject, WeaponData.AttackRange,
			Key_Observation_Self_Dynamic_ActiveWeapon_Range_Ranged,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentLocation);
		AttackRangeObservation = ULearningAgentsObservations::MakeEitherObservation(AgentObject,
			RangedAttackRangeObservation, ELearningAgentsEitherObservation::B, Key_Observation_Self_Dynamic_ActiveWeapon_Range);
	}
	else
	{
		auto MeleeAttackRangeObservation = ULearningAgentsObservations::MakeFloatObservation(AgentObject, WeaponData.AttackRange, Key_Observation_Self_Dynamic_ActiveWeapon_Range_Melee,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentLocation);
		AttackRangeObservation = ULearningAgentsObservations::MakeEitherObservation(AgentObject,
			MeleeAttackRangeObservation, ELearningAgentsEitherObservation::A, Key_Observation_Self_Dynamic_ActiveWeapon_Range);
	}
	
	auto PowerRate = ULearningAgentsObservations::MakeFloatObservation(AgentObject, WeaponData.PowerRate, Key_Observation_Self_Dynamic_ActiveWeapon_PowerRate);
	
	TMap<FName, FLearningAgentsObservationObjectElement> ActiveWeaponObservations =
	{
		{ Key_Observation_Self_Dynamic_ActiveWeapon_Type, ActiveWeaponTypeObservation },
		{ Key_Observation_Self_Dynamic_ActiveWeapon_Mastery, WeaponMasteryObservation },
		{ Key_Observation_Self_Dynamic_ActiveWeapon_Range, AttackRangeObservation},
		{ Key_Observation_Self_Dynamic_ActiveWeapon_PowerRate, PowerRate }
	};
	
	return ULearningAgentsObservations::MakeStructObservation(AgentObject, ActiveWeaponObservations, Key_Observation_Self_Dynamic_ActiveWeapon);
}

TMap<FName, FLearningAgentsObservationSchemaElement> ULearningAgentsInteractor_Combat::GetSelfObservationsSchema(
	ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const
{
	// TODO (aki, 11.11.2025): use RelevantObservations from settings
	
	TMap<FName, FLearningAgentsObservationSchemaElement> Result;
	auto HealthObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.f, Key_Observation_Self_Dynamic_Health);
	auto StaminaObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.f, Key_Observation_Self_Dynamic_Stamina);
	auto VelocityObservation = ULearningAgentsObservations::SpecifyVelocityObservation(InObservationSchema, Settings->MaxSpeed, Key_Observation_Self_Dynamic_Velocity);
	auto StatesObservation = ULearningAgentsObservations::SpecifyBitmaskObservation(InObservationSchema, StaticEnum<ELACharacterStates>(), Key_Observation_Self_Dynamic_CombatStates);

	auto WeaponObservation = GetWeaponObservationSchema(InObservationSchema, Settings);
	
	TMap<FName, FLearningAgentsObservationSchemaElement> DynamicSelfObservations =
	{
		{ Key_Observation_Self_Dynamic_Health, HealthObservation },
		{ Key_Observation_Self_Dynamic_Stamina, StaminaObservation },
		{ Key_Observation_Self_Dynamic_Velocity, VelocityObservation },
		{ Key_Observation_Self_Dynamic_CombatStates, StatesObservation },
		{ Key_Observation_Self_Dynamic_ActiveWeapon, WeaponObservation },
	};
	
	// auto GestureObservation = ULearningAgentsObservations::SpecifyEnumObservation(InObservationSchema, 
	// 	StaticEnum<ELAGesture>(), Key_Observation_Self_Dynamic_Gesture);
	if (!Settings->Gestures.IsEmpty())
	{
		TArray<FName> GestureNames;
		GestureNames.Reserve(Settings->Gestures.Num());
		for (const auto& Gesture : Settings->Gestures)
			GestureNames.Emplace(Gesture.GetTagName());
		
		auto GestureObservation = ULearningAgentsObservations::SpecifyNamedExclusiveDiscreteObservation(InObservationSchema, 
			GestureNames, Key_Observation_Self_Dynamic_Gesture);
		auto GestureOptionalObservation = ULearningAgentsObservations::SpecifyOptionalObservation(InObservationSchema, GestureObservation, 32, Key_Observation_Self_Dynamic_Gesture_Optional);
		
		DynamicSelfObservations.Add(Key_Observation_Self_Dynamic_Gesture_Optional, GestureOptionalObservation);
	}
	
	auto DynamicSelfObservationsStruct = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema,
		DynamicSelfObservations, Key_Observation_Self_Dynamic);

	auto LevelObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, Settings->MaxLevel, Key_Observation_Self_Static_Level);
	auto ArmorRateObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, Settings->MaxArmorRate, Key_Observation_Self_Static_ArmorRate);
	auto SurvivalDesireObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.f, Key_Observation_Self_Static_SurvivalDesire);

	TMap<FName, FLearningAgentsObservationSchemaElement> StaticSelfObservations =
	{
		{ Key_Observation_Self_Static_Level, LevelObservation },
		{ Key_Observation_Self_Static_ArmorRate, ArmorRateObservation },
		{ Key_Observation_Self_Static_SurvivalDesire, SurvivalDesireObservation }
	};
	
	auto StaticSelfObservationsStruct = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema,
		StaticSelfObservations, Key_Observation_Self_Static);
	
	Result.Add(Key_Observation_Self_Static, StaticSelfObservationsStruct);
	Result.Add(Key_Observation_Self_Dynamic, DynamicSelfObservationsStruct);
	return Result;
}

TMap<FName, FLearningAgentsObservationSchemaElement> ULearningAgentsInteractor_Combat::GetLidarObservationsSchema(
	ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const
{
	// welp, until there's no conv3d, LAs will observe convolved "heightmaps" in different plains 
	
	auto CeilingObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, Settings->MaxCeilingHeight,
		Key_Observation_Surrounding_LIDAR_Ceiling);
	
	auto RaindropDownwardObservation = ULearningAgentsObservations::SpecifyLidarObservation(InObservationSchema,
		Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Item);
	int RaindropDownwardResolution = Settings->GetDownwardRaindropResolution();
	auto RaindropDownwardObservations = ULearningAgentsObservations::SpecifyStaticArrayObservation(InObservationSchema,
		RaindropDownwardObservation, RaindropDownwardResolution * RaindropDownwardResolution, Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Array);
	auto RaindropDownwardObservationsConvolved = ULearningAgentsObservations::SpecifyConv2dObservation(InObservationSchema,
		RaindropDownwardObservations, Settings->LidarRaindropDownwardConv2dParams, Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Convolved);
	
	auto RaindropForwardObservation = ULearningAgentsObservations::SpecifyLidarObservation(InObservationSchema,
		Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Item);
	int RaindropForwardResolution = Settings->GetForwardRaindropResolution();
	auto RaindropForwardObservations = ULearningAgentsObservations::SpecifyStaticArrayObservation(InObservationSchema,
		RaindropForwardObservation, RaindropForwardResolution * RaindropForwardResolution, Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Array);
	auto RaindropForwardObservationsConvolved = ULearningAgentsObservations::SpecifyConv2dObservation(InObservationSchema,
		RaindropForwardObservations, Settings->LidarRaindropForwardConv2dParams, Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Convolved);
	
	auto RaindropBackwardObservation = ULearningAgentsObservations::SpecifyLidarObservation(InObservationSchema,
		Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Item);
	int RaindropBackwardResolution = Settings->GetBackwardRaindropResolution();
	auto RaindropBackwardObservations = ULearningAgentsObservations::SpecifyStaticArrayObservation(InObservationSchema,
		RaindropBackwardObservation, RaindropBackwardResolution * RaindropBackwardResolution, Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Array);
	auto RaindropBackwardObservationsConvolved = ULearningAgentsObservations::SpecifyConv2dObservation(InObservationSchema,
		RaindropBackwardObservations, Settings->LidarRaindropBackwardConv2dParams, Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Convolved);

	TMap<FName, FLearningAgentsObservationSchemaElement> Result = {
		{Key_Observation_Surrounding_LIDAR_Ceiling, CeilingObservation},
		{Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Convolved, RaindropDownwardObservationsConvolved},
		{Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Convolved, RaindropForwardObservationsConvolved},
		{Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Convolved, RaindropBackwardObservationsConvolved},
	};
	
	return Result;
}

FLearningAgentsObservationSchemaElement ULearningAgentsInteractor_Combat::GetEnemyObservationSchema(
	ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const
{
	TMap<FName, FLearningAgentsObservationSchemaElement> DynamicObservations;
	TMap<FName, FLearningAgentsObservationSchemaElement> StaticObservations;

	auto EnemyAliveObservation = ULearningAgentsObservations::SpecifyBoolObservation(InObservationSchema, Key_Observation_Enemy_Dynamic_IsAlive);
	auto EnemyHealthObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.f, Key_Observation_Enemy_Dynamic_Health);
	auto EnemyVelocityObservation = ULearningAgentsObservations::SpecifyVelocityObservation(InObservationSchema, Settings->MaxSpeed, Key_Observation_Enemy_Dynamic_Velocity);
	auto EnemyLocationObservation = ULearningAgentsObservations::SpecifyLocationObservation(InObservationSchema, Settings->EnemyDistanceScale, Key_Observation_Enemy_Dynamic_Location);
	auto EnemyOrientationObservation = ULearningAgentsObservations::SpecifyRotationObservation(InObservationSchema, Key_Observation_Enemy_Dynamic_Orientation);
	auto AgentCanSeeEnemyObservation = ULearningAgentsObservations::SpecifyBoolObservation(InObservationSchema, Key_Observation_Enemy_Dynamic_AgentCanSeeEnemy);
	auto AgentCanBeSeenByEnemyObservation = ULearningAgentsObservations::SpecifyBoolObservation(InObservationSchema, Key_Observation_Enemy_Dynamic_EnemyCanSeeAgent);
	auto KillDesireObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.f, Key_Observation_Enemy_Dynamic_KillDesire);
	auto EnemyCombatStatesObservation = ULearningAgentsObservations::SpecifyBitmaskObservation(InObservationSchema, StaticEnum<ELACharacterStates>(), Key_Observation_Enemy_Dynamic_CombatState);
	auto WeaponObservation = GetWeaponObservationSchema(InObservationSchema, Settings);
	
	DynamicObservations =
	{
		{ Key_Observation_Enemy_Dynamic_Health, EnemyHealthObservation},
		{ Key_Observation_Enemy_Dynamic_IsAlive, EnemyAliveObservation},
		{ Key_Observation_Enemy_Dynamic_Velocity, EnemyVelocityObservation },
		{ Key_Observation_Enemy_Dynamic_Location, EnemyLocationObservation },
		{ Key_Observation_Enemy_Dynamic_Orientation, EnemyOrientationObservation },
		{ Key_Observation_Enemy_Dynamic_AgentCanSeeEnemy, AgentCanSeeEnemyObservation },
		{ Key_Observation_Enemy_Dynamic_EnemyCanSeeAgent, AgentCanBeSeenByEnemyObservation },
		{ Key_Observation_Enemy_Dynamic_KillDesire, KillDesireObservation },
		{ Key_Observation_Enemy_Dynamic_CombatState, EnemyCombatStatesObservation },
		{ Key_Observation_Enemy_Dynamic_Weapon, WeaponObservation }
	};
	
	if (!Settings->Gestures.IsEmpty())
	{
		TArray<FName> GestureNames;
		GestureNames.Reserve(Settings->Gestures.Num());
		for (const auto& Gesture : Settings->Gestures)
			GestureNames.Emplace(Gesture.GetTagName());
		
		auto EnemyGestureObservation = ULearningAgentsObservations::SpecifyNamedExclusiveDiscreteObservation(InObservationSchema, 
			GestureNames, Key_Observation_Enemy_Dynamic_Gesture);
		auto GestureOptionalObservation = ULearningAgentsObservations::SpecifyOptionalObservation(InObservationSchema, EnemyGestureObservation, 
			32, Key_Observation_Enemy_Dynamic_Gesture_Optional);
	
		DynamicObservations.Add(Key_Observation_Enemy_Dynamic_Gesture_Optional, GestureOptionalObservation);
	}
	
	
	auto DynamicObservationsObservation = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, DynamicObservations,
		Key_Observation_Enemy_Dynamic_Observations);

	auto LevelObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, Settings->MaxLevel, Key_Observation_Enemy_Static_Level);
	auto EnemyArmorRateObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, Settings->MaxArmorRate, Key_Observation_Enemy_Static_ArmorRate);
	StaticObservations =
	{
		{ Key_Observation_Enemy_Static_Level, LevelObservation },
		{ Key_Observation_Enemy_Static_ArmorRate, EnemyArmorRateObservation }
	};
	
	auto StaticObservationsObservation = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, StaticObservations,
			Key_Observation_Enemy_Static_Observations);

	TMap<FName, FLearningAgentsObservationSchemaElement> EnemyObservations = 
	{
		{ Key_Observation_Enemy_Dynamic, DynamicObservationsObservation },
		{ Key_Observation_Enemy_Static, StaticObservationsObservation }
	};

	return ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, EnemyObservations, Key_Observation_Enemy);
}

FLearningAgentsObservationSchemaElement ULearningAgentsInteractor_Combat::GetAllyObservationsSchema(
	ULearningAgentsObservationSchema* InObservationSchema, const UCombatLearningSettings* Settings) const
{
	// 13.11.2025 (aki): a lot of code is copied from Enemy schema definition.
	// I understand that I could just make uniform "other character" observation and have a field like "attitude"
	// but idk, I have a hunch that having allies and enemies as entirely different observations is better for learning
	// because like it's completely different data subsets rather than having them differ on just 1 enum property ("ally, enemy, neutral") 
	TMap<FName, FLearningAgentsObservationSchemaElement> DynamicObservations;
	TMap<FName, FLearningAgentsObservationSchemaElement> StaticObservations;

	auto AllyAliveObservation = ULearningAgentsObservations::SpecifyBoolObservation(InObservationSchema, Key_Observation_Ally_Dynamic_IsAlive);
	auto AllyHealthObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.f, Key_Observation_Ally_Dynamic_Health);
	auto AllyVelocityObservation = ULearningAgentsObservations::SpecifyVelocityObservation(InObservationSchema, Settings->MaxSpeed, Key_Observation_Ally_Dynamic_Velocity);
	auto AllyLocationObservation = ULearningAgentsObservations::SpecifyLocationObservation(InObservationSchema, Settings->EnemyDistanceScale, Key_Observation_Ally_Dynamic_Location);
	auto AllyOrientationObservation = ULearningAgentsObservations::SpecifyRotationObservation(InObservationSchema, Key_Observation_Ally_Dynamic_Orientation);
	auto AgentCanSeeAllyObservation = ULearningAgentsObservations::SpecifyBoolObservation(InObservationSchema, Key_Observation_Ally_Dynamic_AgentCanSeeAlly);
	auto AllyCombatStatesObservation = ULearningAgentsObservations::SpecifyBitmaskObservation(InObservationSchema, StaticEnum<ELACharacterStates>(), Key_Observation_Ally_Dynamic_CombatState);
	auto WeaponObservation = GetWeaponObservationSchema(InObservationSchema, Settings);
	DynamicObservations =
	{
		{ Key_Observation_Ally_Dynamic_Health, AllyHealthObservation},
		{ Key_Observation_Ally_Dynamic_IsAlive, AllyAliveObservation},
		{ Key_Observation_Ally_Dynamic_Velocity, AllyVelocityObservation },
		{ Key_Observation_Ally_Dynamic_Location, AllyLocationObservation },
		{ Key_Observation_Ally_Dynamic_Orientation, AllyOrientationObservation },
		{ Key_Observation_Ally_Dynamic_AgentCanSeeAlly, AgentCanSeeAllyObservation },
		{ Key_Observation_Ally_Dynamic_CombatState, AllyCombatStatesObservation },
		{ Key_Observation_Ally_Dynamic_Weapon, WeaponObservation }
	};
	
	if (!Settings->Gestures.IsEmpty())
	{
		TArray<FName> GestureNames;
		GestureNames.Reserve(Settings->Gestures.Num());
		for (const auto& Gesture : Settings->Gestures)
			GestureNames.Emplace(Gesture.GetTagName());
		
		auto AllyGestureObservation = ULearningAgentsObservations::SpecifyNamedExclusiveDiscreteObservation(InObservationSchema, GestureNames,
			Key_Observation_Ally_Dynamic_Gesture);
		auto GestureOptionalObservation = ULearningAgentsObservations::SpecifyOptionalObservation(InObservationSchema, AllyGestureObservation, 32, Key_Observation_Ally_Dynamic_Gesture_Optional);
		DynamicObservations.Add(Key_Observation_Ally_Dynamic_Gesture_Optional, GestureOptionalObservation);
	}
	
	auto DynamicObservationsObservation = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, DynamicObservations,
		Key_Observation_Ally_Dynamic_Observations);

	auto LevelObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, Settings->MaxLevel, Key_Observation_Ally_Static_Level);
	auto AllyArmorRateObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, Settings->MaxArmorRate, Key_Observation_Ally_Static_ArmorRate);
	StaticObservations =
	{
		{ Key_Observation_Ally_Static_Level, LevelObservation },
		{ Key_Observation_Ally_Static_ArmorRate, AllyArmorRateObservation }
	};
	
	auto StaticObservationsObservation = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, StaticObservations,
			Key_Observation_Ally_Static_Observations);

	TMap<FName, FLearningAgentsObservationSchemaElement> AllyObservations = 
	{
		{ Key_Observation_Ally_Dynamic, DynamicObservationsObservation },
		{ Key_Observation_Ally_Static, StaticObservationsObservation }
	};

	return ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, AllyObservations, Key_Observation_Ally);
}

TMap<FName, FLearningAgentsObservationSchemaElement> ULearningAgentsInteractor_Combat::GetCombatStateObservationSchema(
	ULearningAgentsObservationSchema* InObservationSchema) const
{
	auto Settings = GetDefault<UCombatLearningSettings>();
	auto CombatTotalDurationObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, Settings->MaxExpectedCombatDuration, Key_Observation_Environment_TotalCombatDuration);
	
	TMap<FName, FLearningAgentsObservationSchemaElement> Result =
	{
		{ Key_Observation_Environment_TotalCombatDuration, CombatTotalDurationObservation },
	};
	
	return Result;
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GatherSelfObservations(
	ULearningAgentsObservationObject* InObservationObject, int AgentId, const FSelfData& SelfData) const
{
	auto MutableThis = const_cast<ULearningAgentsInteractor_Combat*>(this);
	auto Settings = GetDefault<UCombatLearningSettings>();
	auto AgentActor = Cast<AActor>(GetAgent(AgentId));
	auto AgentWorldLocation = AgentActor->GetActorLocation();
	auto AgentTransform = AgentActor->GetTransform();
	
// ==================== gather static observations
	
	auto LevelObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, SelfData.Level, Key_Observation_Self_Static_Level,
		Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
	
	auto ArmorRateObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, SelfData.ArmorRate, Key_Observation_Self_Static_ArmorRate,
		Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
	
	auto SurvivalDesireObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, SelfData.SurvivalDesire, Key_Observation_Self_Static_SurvivalDesire,
		Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
	
	TMap<FName, FLearningAgentsObservationObjectElement> StaticSelfObservations =
	{
		{ Key_Observation_Self_Static_Level, LevelObservation },
		{ Key_Observation_Self_Static_ArmorRate, ArmorRateObservation },
		{ Key_Observation_Self_Static_SurvivalDesire, SurvivalDesireObservation }
	};

// ====================== gather dynamic observations 

	auto HealthObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, SelfData.NormalizedHealth, Key_Observation_Self_Dynamic_Health,
		Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
	auto StaminaObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, SelfData.NormalizedStamina, Key_Observation_Self_Dynamic_Stamina,
		Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
	auto VelocityObservation = ULearningAgentsObservations::MakeVelocityObservation(InObservationObject, SelfData.WorldVelocity, AgentTransform,
		Key_Observation_Self_Dynamic_Velocity,
		Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
	
	auto StatesObservation = ULearningAgentsObservations::MakeBitmaskObservation(InObservationObject,
		StaticEnum<ELACharacterStates>(), static_cast<int32>(SelfData.CombatStates), Key_Observation_Self_Dynamic_CombatStates,
		Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);

	auto WeaponObservation = GetWeaponObservation(InObservationObject, AgentId, SelfData.WeaponData, Settings, AgentWorldLocation);

	FLearningAgentsObservationObjectElement EnemyGestureObservation;
	if (SelfData.ActiveGesture.IsValid())
	{
		EnemyGestureObservation = ULearningAgentsObservations::MakeNamedExclusiveDiscreteObservation(InObservationObject,
			SelfData.ActiveGesture.GetTagName(), Key_Observation_Self_Dynamic_Gesture,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
	}
		
	ELearningAgentsOptionalObservation GestureOptionalObservationStatus = SelfData.ActiveGesture.IsValid()
		? ELearningAgentsOptionalObservation::Valid
		: ELearningAgentsOptionalObservation::Null;
	auto GestureOptionalObservation = ULearningAgentsObservations::MakeOptionalObservation(InObservationObject, EnemyGestureObservation,
		GestureOptionalObservationStatus, Key_Observation_Self_Dynamic_Gesture_Optional);
	
	TMap<FName, FLearningAgentsObservationObjectElement> DynamicSelfObservations =
	{
		{ Key_Observation_Self_Dynamic_Health, HealthObservation },
		{ Key_Observation_Self_Dynamic_Stamina, StaminaObservation },
		{ Key_Observation_Self_Dynamic_Velocity, VelocityObservation },
		{ Key_Observation_Self_Dynamic_CombatStates, StatesObservation },
		{ Key_Observation_Self_Dynamic_ActiveWeapon, WeaponObservation },
		{ Key_Observation_Self_Dynamic_Gesture_Optional, GestureOptionalObservation }
	};
	
// ====================== combine static and dynamic observations
	
	auto StaticSelfObservationsObservation = ULearningAgentsObservations::MakeStructObservation(InObservationObject, StaticSelfObservations, Key_Observation_Self_Static);
	auto DynamicSelfObservationsObservation = ULearningAgentsObservations::MakeStructObservation(InObservationObject, DynamicSelfObservations, Key_Observation_Self_Dynamic);
	
	TMap<FName, FLearningAgentsObservationObjectElement> Observations =
	{
		{ Key_Observation_Self_Static, StaticSelfObservationsObservation },
		{ Key_Observation_Self_Dynamic, DynamicSelfObservationsObservation }
	};
	
	return ULearningAgentsObservations::MakeStructObservation(InObservationObject, Observations, Key_Observation_Self);
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GatherSurroundingsObservations(ULearningAgentsObservationObject* InObservationObject,
	int32 AgentId, ULearningAgentCombatObservationComponent* LAObservationComponent)
{
	// welp, until there's no conv3d, LAs will observe convolved "heightmaps" in different plains 
	TMap<FName, FLearningAgentsObservationObjectElement> ResultMap;
	
	const FLidarObservationCache& LidarData = LAObservationComponent->GetLidarData();
	auto CeilingObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, LidarData.AverageCeilingHeight,
		Key_Observation_Surrounding_LIDAR_Ceiling);
	
	TArray<FLearningAgentsObservationObjectElement> RaindropDownwardObservations;
	RaindropDownwardObservations.SetNum(LidarData.DownwardRaindrops.Num());
	for (int i = 0; i < LidarData.DownwardRaindrops.Num(); i++)
	{
		auto RaindropDownHeightmapItemObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject,
			LidarData.DownwardRaindrops[i], Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Item);
		RaindropDownwardObservations[i] = RaindropDownHeightmapItemObservation;
	}
	
	auto RaindropDownwardObservationsObservation = ULearningAgentsObservations::MakeStaticArrayObservation(InObservationObject, 
		RaindropDownwardObservations, Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Array);
	auto RaindropDownwardObservationsConvolved = ULearningAgentsObservations::MakeConv2dObservation(InObservationObject,
		RaindropDownwardObservationsObservation, Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Convolved);
	
	TArray<FLearningAgentsObservationObjectElement> RaindropForwardObservations;
	RaindropForwardObservations.SetNum(LidarData.ForwardRaindrops.Num());
	for (int i = 0; i < LidarData.ForwardRaindrops.Num(); i++)
	{
		auto RaindropForwardHeightmapItemObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject,
			LidarData.ForwardRaindrops[i], Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Item);
		RaindropForwardObservations[i] = RaindropForwardHeightmapItemObservation;
	}
	
	auto RaindropForwardObservationsObservation = ULearningAgentsObservations::MakeStaticArrayObservation(InObservationObject, 
		RaindropForwardObservations, Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Array);
	auto RaindropForwardObservationsConvolved = ULearningAgentsObservations::MakeConv2dObservation(InObservationObject,
		RaindropForwardObservationsObservation, Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Convolved);
	
	TArray<FLearningAgentsObservationObjectElement> RaindropBackwardObservations;
	RaindropBackwardObservations.SetNum(LidarData.BackwardRaindrops.Num());
	for (int i = 0; i < LidarData.BackwardRaindrops.Num(); i++)
	{
		auto RaindropBackwardHeightmapItemObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject,
			LidarData.BackwardRaindrops[i], Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Item);
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
	auto MutableThis = const_cast<ULearningAgentsInteractor_Combat*>(this);
	auto Settings = GetDefault<UCombatLearningSettings>();
	auto AgentActor = Cast<AActor>(GetAgent(AgentId));
	auto AgentWorldLocation = AgentActor->GetActorLocation();
	
	auto CombatTotalDurationObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, CombatStateData.CombatTotalDuration,
		Key_Observation_Environment_TotalCombatDuration,
		Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
	TMap<FName, FLearningAgentsObservationObjectElement> Observations =
	{
		{ Key_Observation_Environment_TotalCombatDuration, CombatTotalDurationObservation }	
	};
	
	return ULearningAgentsObservations::MakeStructObservation(InObservationObject, Observations, Key_Observation_Environment);
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GatherEnemiesObservation(ULearningAgentsObservationObject* InObservationObject,
	int32 AgentId, const TArray<FEnemyData>& EnemiesData)
{
	auto MutableThis = const_cast<ULearningAgentsInteractor_Combat*>(this);
	auto Settings = GetDefault<UCombatLearningSettings>();
	auto AgentActor = Cast<AActor>(GetAgent(AgentId));
	auto AgentWorldLocation = AgentActor->GetActorLocation();
	auto AgentTransform = AgentActor->GetActorTransform();
	auto AgentRotation = AgentActor->GetActorRotation();

	TArray<FLearningAgentsObservationObjectElement> EnemiesObservations;
	
	for (const auto& EnemyData : EnemiesData)
	{
		TMap<FName, FLearningAgentsObservationObjectElement> DynamicObservations;
		TMap<FName, FLearningAgentsObservationObjectElement> StaticObservations;

		auto EnemyAliveObservation = ULearningAgentsObservations::MakeBoolObservation(InObservationObject, EnemyData.bAlive,
			Key_Observation_Enemy_Dynamic_IsAlive,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		auto EnemyHealthObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, EnemyData.NormalizedHealth,
			Key_Observation_Enemy_Dynamic_Health,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		auto EnemyVelocityObservation = ULearningAgentsObservations::MakeVelocityObservation(InObservationObject, EnemyData.WorldVelocity, AgentTransform,
			Key_Observation_Enemy_Dynamic_Velocity,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		auto EnemyLocationObservation = ULearningAgentsObservations::MakeLocationObservation(InObservationObject,
			EnemyData.Actor->GetActorLocation(), AgentTransform,
			Key_Observation_Enemy_Dynamic_Location,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		auto EnemyOrientationObservation = ULearningAgentsObservations::MakeRotationObservation(InObservationObject,
			EnemyData.Actor->GetActorRotation(), AgentRotation,
			Key_Observation_Enemy_Dynamic_Orientation,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		auto AgentCanSeeEnemyObservation = ULearningAgentsObservations::MakeBoolObservation(InObservationObject, EnemyData.bAgentCanSeeCharacter,
			Key_Observation_Enemy_Dynamic_AgentCanSeeEnemy,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		auto EnemyCanSeeAgentObservation = ULearningAgentsObservations::MakeBoolObservation(InObservationObject, EnemyData.bCharacterCanSeeAgent,
			Key_Observation_Enemy_Dynamic_EnemyCanSeeAgent,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		auto KillDesireObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, EnemyData.KillDesire,
			Key_Observation_Enemy_Dynamic_KillDesire,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		
		FLearningAgentsObservationObjectElement EnemyGestureObservation;
		if (EnemyData.ActiveGesture.IsValid())
		{
			EnemyGestureObservation = ULearningAgentsObservations::MakeNamedExclusiveDiscreteObservation(InObservationObject,
				EnemyData.ActiveGesture.GetTagName() , Key_Observation_Enemy_Dynamic_Gesture,
				Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		}
		
		ELearningAgentsOptionalObservation GestureOptionalObservationStatus = EnemyData.ActiveGesture.IsValid()
			? ELearningAgentsOptionalObservation::Valid
			: ELearningAgentsOptionalObservation::Null;
		auto GestureOptionalObservation = ULearningAgentsObservations::MakeOptionalObservation(InObservationObject, EnemyGestureObservation,
			GestureOptionalObservationStatus, Key_Observation_Enemy_Dynamic_Gesture_Optional);
		
		auto EnemyCombatStatesObservation = ULearningAgentsObservations::MakeBitmaskObservation(InObservationObject,
			StaticEnum<ELACharacterStates>(),static_cast<int32>(EnemyData.CombatStates),
			Key_Observation_Enemy_Dynamic_CombatState,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		auto WeaponObservation = GetWeaponObservation(InObservationObject, AgentId, EnemyData.WeaponData, Settings, AgentWorldLocation);

		DynamicObservations =
		{
			{ Key_Observation_Enemy_Dynamic_Health, EnemyHealthObservation},
			{ Key_Observation_Enemy_Dynamic_IsAlive, EnemyAliveObservation},
			{ Key_Observation_Enemy_Dynamic_Velocity, EnemyVelocityObservation },
			{ Key_Observation_Enemy_Dynamic_Location, EnemyLocationObservation },
			{ Key_Observation_Enemy_Dynamic_Orientation, EnemyOrientationObservation },
			{ Key_Observation_Enemy_Dynamic_AgentCanSeeEnemy, AgentCanSeeEnemyObservation },
			{ Key_Observation_Enemy_Dynamic_EnemyCanSeeAgent, EnemyCanSeeAgentObservation },
			{ Key_Observation_Enemy_Dynamic_KillDesire, KillDesireObservation },
			{ Key_Observation_Enemy_Dynamic_Gesture_Optional, GestureOptionalObservation },
			{ Key_Observation_Enemy_Dynamic_CombatState, EnemyCombatStatesObservation },
			{ Key_Observation_Enemy_Dynamic_Weapon, WeaponObservation }
		};
		
		auto DynamicObservationsObservation = ULearningAgentsObservations::MakeStructObservation(InObservationObject, DynamicObservations,
			Key_Observation_Enemy_Dynamic_Observations);

		auto LevelObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, EnemyData.Level,
			Key_Observation_Enemy_Static_Level,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		auto EnemyArmorRateObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, EnemyData.ArmorRate,
			Key_Observation_Enemy_Static_ArmorRate,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		
		StaticObservations =
		{
			{ Key_Observation_Enemy_Static_Level, LevelObservation },
			{ Key_Observation_Enemy_Static_ArmorRate, EnemyArmorRateObservation }
		};
		
		auto StaticObservationsObservation = ULearningAgentsObservations::MakeStructObservation(InObservationObject, StaticObservations,
			Key_Observation_Enemy_Static_Observations);

		TMap<FName, FLearningAgentsObservationObjectElement> EnemyObservations = 
		{
			{ Key_Observation_Enemy_Dynamic, DynamicObservationsObservation },
			{ Key_Observation_Enemy_Static, StaticObservationsObservation }
		};

		auto EnemyObservationsObservation = ULearningAgentsObservations::MakeStructObservation(InObservationObject, EnemyObservations, Key_Observation_Enemy);
		EnemiesObservations.Add(EnemyObservationsObservation);
	}

	return ULearningAgentsObservations::MakeArrayObservation(InObservationObject, EnemiesObservations, Settings->MaxEnemiesObservedAtOnce, Key_Observation_Enemies);
}

FLearningAgentsObservationObjectElement ULearningAgentsInteractor_Combat::GatherAlliesObservations(ULearningAgentsObservationObject* InObservationObject,
	int32 AgentId, const TArray<FAllyData>& AlliesData)
{
	auto MutableThis = const_cast<ULearningAgentsInteractor_Combat*>(this);
	auto Settings = GetDefault<UCombatLearningSettings>();
	auto AgentActor = Cast<AActor>(GetAgent(AgentId));
	auto AgentWorldLocation = AgentActor->GetActorLocation();
	auto AgentTransform = AgentActor->GetActorTransform();
	auto AgentRotation = AgentActor->GetActorRotation();

	TArray<FLearningAgentsObservationObjectElement> AlliesObservations;
	
	for (const auto& AllyData : AlliesData)
	{
		TMap<FName, FLearningAgentsObservationObjectElement> DynamicObservations;
		TMap<FName, FLearningAgentsObservationObjectElement> StaticObservations;

		auto AllyAliveObservation = ULearningAgentsObservations::MakeBoolObservation(InObservationObject, AllyData.bAlive,
			Key_Observation_Ally_Dynamic_IsAlive,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		auto AllyHealthObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, AllyData.NormalizedHealth,
			Key_Observation_Ally_Dynamic_Health,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		auto AllyVelocityObservation = ULearningAgentsObservations::MakeVelocityObservation(InObservationObject,
			AllyData.WorldVelocity, AgentTransform,
			Key_Observation_Ally_Dynamic_Velocity,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		auto AllyLocationObservation = ULearningAgentsObservations::MakeLocationObservation(InObservationObject,
			AllyData.AllyActor->GetActorLocation(), AgentTransform,
			Key_Observation_Ally_Dynamic_Location,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		auto AllyOrientationObservation = ULearningAgentsObservations::MakeRotationObservation(InObservationObject,
			AllyData.AllyActor->GetActorRotation(), AgentRotation,
			Key_Observation_Ally_Dynamic_Orientation,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		auto AgentCanSeeAllyObservation = ULearningAgentsObservations::MakeBoolObservation(InObservationObject, AllyData.bAgentCanSeeCharacter,
			Key_Observation_Ally_Dynamic_AgentCanSeeAlly,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		
		FLearningAgentsObservationObjectElement GestureObservation;
		if (AllyData.ActiveGesture.IsValid())
		{
			GestureObservation = ULearningAgentsObservations::MakeNamedExclusiveDiscreteObservation(InObservationObject,
				AllyData.ActiveGesture.GetTagName(), Key_Observation_Ally_Dynamic_Gesture,
				Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		}
		
		ELearningAgentsOptionalObservation GestureOptionalObservationStatus = AllyData.ActiveGesture.IsValid()
			? ELearningAgentsOptionalObservation::Valid
			: ELearningAgentsOptionalObservation::Null;
		auto GestureOptionalObservation = ULearningAgentsObservations::MakeOptionalObservation(InObservationObject, GestureObservation,
			GestureOptionalObservationStatus, Key_Observation_Ally_Dynamic_Gesture_Optional);
		
		auto AllyCombatStatesObservation = ULearningAgentsObservations::MakeBitmaskObservation(InObservationObject,
			StaticEnum<ELACharacterStates>(), static_cast<int32>(AllyData.CombatStates),
			Key_Observation_Ally_Dynamic_CombatState,
			Settings->bVisLogEnabled, MutableThis, AgentId, AgentWorldLocation);
		auto WeaponObservation = GetWeaponObservation(InObservationObject, AgentId, AllyData.WeaponData, Settings, AgentWorldLocation);

		DynamicObservations =
		{
			{ Key_Observation_Ally_Dynamic_Health, AllyHealthObservation},
			{ Key_Observation_Ally_Dynamic_IsAlive, AllyAliveObservation},
			{ Key_Observation_Ally_Dynamic_Velocity, AllyVelocityObservation },
			{ Key_Observation_Ally_Dynamic_Location, AllyLocationObservation },
			{ Key_Observation_Ally_Dynamic_Orientation, AllyOrientationObservation },
			{ Key_Observation_Ally_Dynamic_AgentCanSeeAlly, AgentCanSeeAllyObservation },
			{ Key_Observation_Ally_Dynamic_Gesture_Optional, GestureOptionalObservation },
			{ Key_Observation_Ally_Dynamic_CombatState, AllyCombatStatesObservation },
			{ Key_Observation_Ally_Dynamic_Weapon, WeaponObservation }
		};
	
		auto DynamicObservationsObservation = ULearningAgentsObservations::MakeStructObservation(InObservationObject, DynamicObservations,
			Key_Observation_Ally_Dynamic_Observations);

		auto LevelObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, AllyData.Level, Key_Observation_Ally_Static_Level);
		auto AllyArmorRateObservation = ULearningAgentsObservations::MakeFloatObservation(InObservationObject, AllyData.ArmorRate, Key_Observation_Ally_Static_ArmorRate);
		StaticObservations =
		{
			{ Key_Observation_Ally_Static_Level, LevelObservation },
			{ Key_Observation_Ally_Static_ArmorRate, AllyArmorRateObservation }
		};
	
		auto StaticObservationsObservation = ULearningAgentsObservations::MakeStructObservation(InObservationObject, StaticObservations,
				Key_Observation_Ally_Static_Observations);

		TMap<FName, FLearningAgentsObservationObjectElement> AllyObservations = 
		{
			{ Key_Observation_Ally_Dynamic, DynamicObservationsObservation },
			{ Key_Observation_Ally_Static, StaticObservationsObservation }
		};

		auto AllyObservationsObservation = ULearningAgentsObservations::MakeStructObservation(InObservationObject, AllyObservations,
			Key_Observation_Ally);
		AlliesObservations.Add(AllyObservationsObservation);
	}

	return ULearningAgentsObservations::MakeArrayObservation(InObservationObject, AlliesObservations, Settings->MaxAlliesObservedAtOnce, Key_Observation_Allies);
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
		Key_Observation_Self_Dynamic);
	if (!bFoundDynamicSelfObservation)
	{
		ensure(false);
		return false;
	}
	
	auto SelfStatesObservation = DynamicSelfObservations.Find(Key_Observation_Self_Dynamic_CombatStates);
	if (SelfStatesObservation == nullptr)
	{
		ensure(false);
		return false;
	}

	OutSelfStates = ELACharacterStates::None;
	int32 RawSelfStates = 0;
	bool bFoundSelfStates = ULearningAgentsObservations::GetBitmaskObservation(RawSelfStates, InObservationObject, *SelfStatesObservation,
																			   StaticEnum<ELACharacterStates>(), Key_Observation_Self_Dynamic_CombatStates);
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
	if ((SelfStates & ELACharacterStates::Attacking) != 0)
	{
		// idk, it can actually depend on the archetype 
		Result.Emplace(Key_Action_Combat_Dodge);
		Result.Emplace(Key_Actions_Locomotion);
	}
	
	if ((SelfStates & ELACharacterStates::InStagger) != 0)
	{
		Result.Emplace(Key_Actions_Locomotion);
		Result.Emplace(Key_Action_Combat);
	}
	
	if ((SelfStates & ELACharacterStates::Parrying) != 0)
	{
		Result.Emplace(Key_Action_Combat_Attack);
		Result.Emplace(Key_Action_Combat_Dodge);
		Result.Emplace(Key_Action_Gesture);
		Result.Emplace(Key_Action_UseConsumableItem);
		Result.Emplace(Key_Action_Locomotion_Climb);
	}
	
	if ((SelfStates & ELACharacterStates::Blocking) != 0)
	{
		Result.Emplace(Key_Action_Gesture);
		Result.Emplace(Key_Action_UseConsumableItem);
		Result.Emplace(Key_Action_Locomotion_Climb);
	}
	
	if ((SelfStates & ELACharacterStates::Dodging) != 0)
	{
		Result.Emplace(Key_Action_Combat_Attack);
		Result.Emplace(Key_Actions_Locomotion);
	}
	
	if ((SelfStates & ELACharacterStates::Dying) != 0)
	{
		Result.Emplace(Key_Action_Locomotion_Climb);
		Result.Emplace(Key_Action_Locomotion_Jump);
		Result.Emplace(Key_Action_Combat_Parry);
		Result.Emplace(Key_Action_Combat_Dodge);
	}
	
	if ((SelfStates & ELACharacterStates::Falling) != 0)
	{
		Result.Emplace(Key_Action_Combat_Dodge);
		Result.Emplace(Key_Action_Locomotion_SetSpeed);
		Result.Emplace(Key_Action_Locomotion_Move);
	}
	
	if ((SelfStates & ELACharacterStates::Swimming) != 0)
	{
		Result.Emplace(Key_Action_Locomotion_Jump);
		Result.Emplace(Key_Action_Combat);
		Result.Emplace(Key_Action_Gesture);
	}
	
	if ((SelfStates & ELACharacterStates::WeaponSheathed) != 0)
	{
		Result.Emplace(Key_Action_Combat_Attack);	
		Result.Emplace(Key_Action_Combat_Parry);	
	}
	
	if ((SelfStates & ELACharacterStates::Gesturing) != 0)
	{
		Result.Emplace(Key_Action_Gesture);
	}

	if ((SelfStates & ELACharacterStates::Speaking) != 0)
	{
		Result.Emplace(Key_Action_SayPhrase);
	}
	
	if ((SelfStates & ELACharacterStates::UsingConsumableItem) != 0)
	{
		Result.Emplace(Key_Action_SayPhrase);
		Result.Emplace(Key_Action_Gesture);
		Result.Emplace(Key_Action_Locomotion_Climb);
	}
	
	if ((SelfStates & ELACharacterStates::HasActiveRootMotionAnimation) != 0)
	{
		Result.Emplace(Key_Action_Locomotion_Move);
		Result.Emplace(Key_Action_Locomotion_SetSpeed);
	}
	
	if ((SelfStates & ELACharacterStates::InteractingWithObject) != 0)
	{
		Result.Emplace(Key_Actions_Locomotion);
		Result.Emplace(Key_Action_Gesture);
		Result.Emplace(Key_Action_UseConsumableItem);
	}
	
	return Result;
}

// 


#include "LearningEntities/Interactors/LearningAgentsInteractor_Combat.h"

#include "Data/LearningAgentsDataTypes.h"
#include "Interfaces/LearningAgentObservation.h"

FName Key_Observation_Health = FName("Observation.Status.Self.Health");
FName Key_CombatObservations = FName("Observation.Combat.All");
FName Key_Observation_Self = FName("Observation.Self");
FName Key_Observation_LIDAR = FName("Observation.LIDAR");
FName Key_Observation_Enemies = FName("Observation.Enemies");
FName Key_Observation_Allies = FName("Observation.Allies");
FName Key_Observation_Environment = FName("Observation.Environment");
FName Key_Observation_Self_Static = FName("Observation.Self.Static");

FName Key_Observation_Self_Dynamic = FName("Observation.Self.Dynamic");
FName Key_Observation_Self_Dynamic_Health = FName("Observation.Self.Dynamic.Health");
FName Key_Observation_Self_Dynamic_Stamina = FName("Observation.Self.Dynamic.Stamina");
FName Key_Observation_Self_Dynamic_Speed = FName("Observation.Self.Dynamic.Speed");
FName Key_Observation_Self_Dynamic_States = FName("Observation.Self.Dynamic.States");

FName Key_Observation_Self_Dynamic_ActiveWeapon = FName("Observation.Self.Dynamic.ActiveWeapon.Range");
FName Key_Observation_Self_Dynamic_ActiveWeapon_Range = FName("Observation.Self.Dynamic.ActiveWeapon.Range");
FName Key_Observation_Self_Dynamic_ActiveWeapon_Type = FName("Observation.Self.Dynamic.ActiveWeapon.Type");
FName Key_Observation_Self_Dynamic_ActiveWeapon_Mastery = FName("Observation.Self.Dynamic.ActiveWeapon.Mastery");

/*
 * Combat observation hierarchy
 * All
 *		Self
 *			Dynamic
 *				fHealth
 *				fStamina
 *				fMoveSpeed
 *				eCombatState (none, attacking, dodging, staggered, blocking, parried attack, sprinting, sneaking)
 *				tWeapon
 *					fAttackRange
 *					fWeaponMastery
 *					eWeaponType (unarmed, melee, ranged)
 *			Static	
 *				iLevel
 *				fArmorRate
 *				fPowerRate
 *				eFaction
 *				eConceptualToughness (e.g. paladin vs regular bandit)
 *				fSurviveDesire
 *			
 *		LIDAR
 *			Walkable surfaces
 *			Walls
 *			Ceilings
 *			Vault points
 *			Climb points
 *		Enemies
 *			Dynamic
 *				iEnemyId (assume each enemy in current combat situation has an id to distinguish them for learning purposes)
 *				bAlive
 *				fHealth
 *				fMoveSpeed
 *				vRelativeOrientation
 *				vKnownRelativeLocation
 *				vAssumedRelativeLocation
 *				bCanSee
 *				bCanBeSeen
 *				fKillDesire
*				eActiveGesture
*				eActivePhrase
*				bReachable				
*				eCombatState (none, attacking, dodging, staggered, blocking, parried attack, sprinting, sneaking)
*				tWeapon
*					fAttackRange
*					fWeaponMastery
*					eWeaponType (unarmed, melee, ranged)
*			Static
*				fArmorRate (assumed)
*				fPowerRate (assumed)
*				eFaction
*				
 *		Allies
 *			Dynamic
 *				bAlive
 *				fHealth
 *				fMoveSpeed
 *				vRelativeOrientation
 *				vAssumedRelativeLocation
 *				vKnownRelativeLocation
*				eActiveGesture
*				eActivePhrase
*				bInCombat
*				dActiveAllyId
*				eCombatState (none, attacking, dodging, staggered, blocking, parried attack)
*				fAttackRange
*				eWeaponType (unarmed, melee, ranged)
*			Static
*				fArmorRate
*				fPowerRate
*				eConceptualToughness (e.g. paladin vs regular bandit)
*		Neutrals (just neutral characters, think of citizens, non-aggressive animals, bypassers)
*			Dynamic
*			//... same as allies
*			Static
*			//... same as allies
*			
*		Environment
*			fCombatTime
*			fTimeOfDay
*			iAlliesAround
 */

void ULearningAgentsInteractor_Combat::SpecifyAgentObservation_Implementation(
	FLearningAgentsObservationSchemaElement& OutObservationSchemaElement,
	ULearningAgentsObservationSchema* InObservationSchema)
{
	Super::SpecifyAgentObservation_Implementation(OutObservationSchemaElement, InObservationSchema);
	TMap<FName, FLearningAgentsObservationSchemaElement> Observations;

	auto SelfObservations = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema,
		GetSelfObservations(InObservationSchema), Key_Observation_Self);
	auto LidarObservations = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema,
		GetLidarObservations(InObservationSchema), Key_Observation_LIDAR);
	auto EnemiesObservations = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema,
		GetEnemiesObservations(InObservationSchema), Key_Observation_Enemies);
	auto AlliesObservations = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema,
		GetAlliesObservations(InObservationSchema), Key_Observation_Allies);
	auto EnvironmentObservations = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema,
		GetEnvironmentObservations(InObservationSchema), Key_Observation_Environment);

	Observations.Add(Key_Observation_Self, SelfObservations);
	Observations.Add(Key_Observation_LIDAR, LidarObservations);
	Observations.Add(Key_Observation_Enemies, EnemiesObservations);
	Observations.Add(Key_Observation_Allies, AlliesObservations);
	Observations.Add(Key_Observation_Environment, EnvironmentObservations);
	
	OutObservationSchemaElement = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, Observations, Key_CombatObservations);
}

void ULearningAgentsInteractor_Combat::SpecifyAgentAction_Implementation(
	FLearningAgentsActionSchemaElement& OutActionSchemaElement, ULearningAgentsActionSchema* InActionSchema)
{
	Super::SpecifyAgentAction_Implementation(OutActionSchemaElement, InActionSchema);
}

void ULearningAgentsInteractor_Combat::GatherAgentObservation_Implementation(
	FLearningAgentsObservationObjectElement& OutObservationObjectElement,
	ULearningAgentsObservationObject* InObservationObject, const int32 AgentId)
{
	Super::GatherAgentObservation_Implementation(OutObservationObjectElement, InObservationObject, AgentId);
	auto Agent = GetAgent(AgentId);
	auto LearningAgentInterface = Cast<ILearningAgentObservation>(Agent);
	if (!ensure(LearningAgentInterface))
		return;
}

void ULearningAgentsInteractor_Combat::PerformAgentAction_Implementation(
	const ULearningAgentsActionObject* InActionObject, const FLearningAgentsActionObjectElement& InActionObjectElement,
	const int32 AgentId)
{
	Super::PerformAgentAction_Implementation(InActionObject, InActionObjectElement, AgentId);
}

TMap<FName, FLearningAgentsObservationSchemaElement> ULearningAgentsInteractor_Combat::GetSelfObservations(
	ULearningAgentsObservationSchema* InObservationSchema) const
{
	TMap<FName, FLearningAgentsObservationSchemaElement> Result;
	TMap<FName, FLearningAgentsObservationSchemaElement> DynamicSelfObservations;
	TMap<FName, FLearningAgentsObservationSchemaElement> StaticSelfObservations;
	
	auto HealthObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.f, Key_Observation_Self_Dynamic_Health);
	auto StaminaObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.f, Key_Observation_Self_Dynamic_Stamina);
	auto MoveSpeedObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1000.f, Key_Observation_Self_Dynamic_Speed);
	auto StatesObservation = ULearningAgentsObservations::SpecifyBitmaskObservation(InObservationSchema, StaticEnum<ELACombatStates>(), Key_Observation_Self_Dynamic_States);

	auto ActiveWeaponTypeObservation = ULearningAgentsObservations::SpecifyEnumObservation(InObservationSchema, StaticEnum<ELAActiveWeaponType>(), Key_Observation_Self_Dynamic_ActiveWeapon_Type);
	auto WeaponMasteryObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.f, Key_Observation_Self_Dynamic_ActiveWeapon_Mastery);
	auto AttackRangeObservation = ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.f, Key_Observation_Self_Dynamic_ActiveWeapon_Range);
	TMap<FName, FLearningAgentsObservationSchemaElement> ActiveWeaponObservations =
	{
		{ Key_Observation_Self_Dynamic_ActiveWeapon_Type, ActiveWeaponTypeObservation },
		{ Key_Observation_Self_Dynamic_ActiveWeapon_Mastery, WeaponMasteryObservation },
		{ Key_Observation_Self_Dynamic_ActiveWeapon_Range, AttackRangeObservation}
	};
	
	auto WeaponObservation = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, ActiveWeaponObservations, Key_Observation_Self_Dynamic_ActiveWeapon);
	
	DynamicSelfObservations.Add(Key_Observation_Self_Dynamic_Health, HealthObservation);
	DynamicSelfObservations.Add(Key_Observation_Self_Dynamic_Stamina, StaminaObservation);
	DynamicSelfObservations.Add(Key_Observation_Self_Dynamic_Speed, MoveSpeedObservation);
	DynamicSelfObservations.Add(Key_Observation_Self_Dynamic_States, StatesObservation);
	DynamicSelfObservations.Add(Key_Observation_Self_Dynamic_ActiveWeapon, WeaponObservation);

	auto DynamicSelfObservationsStruct = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema,
		DynamicSelfObservations, Key_Observation_Self_Dynamic);
	auto StaticSelfObservationsStruct = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema,
		StaticSelfObservations, Key_Observation_Self_Static);

	Result.Add(Key_Observation_Self_Dynamic, DynamicSelfObservationsStruct);
	Result.Add(Key_Observation_Self_Static, StaticSelfObservationsStruct);
	return Result;
}

TMap<FName, FLearningAgentsObservationSchemaElement> ULearningAgentsInteractor_Combat::GetLidarObservations(
	ULearningAgentsObservationSchema* InObservationSchema) const
{
	TMap<FName, FLearningAgentsObservationSchemaElement> Result;
	return Result;
}

TMap<FName, FLearningAgentsObservationSchemaElement> ULearningAgentsInteractor_Combat::GetEnemiesObservations(
	ULearningAgentsObservationSchema* InObservationSchema) const
{
	TMap<FName, FLearningAgentsObservationSchemaElement> Result;
	return Result;
}

TMap<FName, FLearningAgentsObservationSchemaElement> ULearningAgentsInteractor_Combat::GetAlliesObservations(
	ULearningAgentsObservationSchema* InObservationSchema) const
{
	TMap<FName, FLearningAgentsObservationSchemaElement> Result;
	return Result;
}

TMap<FName, FLearningAgentsObservationSchemaElement> ULearningAgentsInteractor_Combat::GetEnvironmentObservations(
	ULearningAgentsObservationSchema* InObservationSchema) const
{
	TMap<FName, FLearningAgentsObservationSchemaElement> Result;
	return Result;
}

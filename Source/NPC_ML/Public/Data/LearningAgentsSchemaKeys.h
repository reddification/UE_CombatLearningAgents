#pragma once

namespace LAObservationKeys
{
	inline const FName Key_Observation_Health = FName("Observation.Status.Self.Health");
	inline const FName Key_CombatObservations = FName("Observation.Combat.All");
	inline const FName Key_Observation_Self = FName("Observation.Self");

	inline const FName Key_Observation_Surrounding = FName("Observation.Surrounding");
	inline const FName Key_Observation_Surrounding_LIDAR = FName("Observation.Surrounding.LIDAR");
	inline const FName Key_Observation_Surrounding_LIDAR_Ceiling = FName("Observation.Surrounding.LIDAR.Ceiling");
	inline const FName Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Item = FName("Observation.Surrounding.LIDAR.Raindrop.Downward.Item");
	inline const FName Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Array = FName("Observation.Surrounding.LIDAR.Raindrop.Downward.Array");
	inline const FName Key_Observation_Surrounding_LIDAR_Raindrop_Downward_Convolved = FName("Observation.Surrounding.LIDAR.Raindrop.Downward.Convolved");
	inline const FName Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Item = FName("Observation.Surrounding.LIDAR.Raindrop.Forward.Item");
	inline const FName Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Array = FName("Observation.Surrounding.LIDAR.Raindrop.Forward.Array");
	inline const FName Key_Observation_Surrounding_LIDAR_Raindrop_Forward_Convolved = FName("Observation.Surrounding.LIDAR.Raindrop.Forward.Convolved");
	inline const FName Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Item = FName("Observation.Surrounding.LIDAR.Raindrop.Backward.Item");
	inline const FName Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Array = FName("Observation.Surrounding.LIDAR.Raindrop.Backward.Array");
	inline const FName Key_Observation_Surrounding_LIDAR_Raindrop_Backward_Convolved = FName("Observation.Surrounding.LIDAR.Raindrop.Backward.Convolved");

	inline const FName Key_Observation_Allies = FName("Observation.Allies");
	inline const FName Key_Observation_Combat_State = FName("Observation.Combat.State");

	inline const FName Key_Observation_Self_Dynamic = FName("Observation.Self.Dynamic");
	inline const FName Key_Observation_Self_Dynamic_Health = FName("Observation.Self.Dynamic.Health");
	inline const FName Key_Observation_Self_Dynamic_Stamina = FName("Observation.Self.Dynamic.Stamina");
	inline const FName Key_Observation_Self_Dynamic_Velocity = FName("Observation.Self.Dynamic.Velocity");
	inline const FName Key_Observation_Self_Dynamic_CombatStates = FName("Observation.Self.Dynamic.CombatStates");
	
	inline const FName Key_Observation_Self_Dynamic_Gesture = FName("Observation.Self.Dynamic.Gesture.");
	inline const FName Key_Observation_Self_Dynamic_Gesture_Optional = FName("Observation.Self.Dynamic.Gesture.Optional");
	inline const FName Key_Observation_Self_Dynamic_Phrase = FName("Observation.Self.Dynamic.Phrase");
	inline const FName Key_Observation_Self_Dynamic_Phrase_Optional = FName("Observation.Self.Dynamic.Phrase.Optional");
	
	inline const FName Key_Observation_Self_Dynamic_ActiveWeapon = FName("Observation.Self.Dynamic.ActiveWeapon.Range");
	inline const FName Key_Observation_Self_Dynamic_ActiveWeapon_Range = FName("Observation.Self.Dynamic.ActiveWeapon.Range");
	inline const FName Key_Observation_Self_Dynamic_ActiveWeapon_Range_Melee = FName("Observation.Self.Dynamic.ActiveWeapon.Range.Melee");
	inline const FName Key_Observation_Self_Dynamic_ActiveWeapon_Range_Ranged = FName("Observation.Self.Dynamic.ActiveWeapon.Range.Ranged");
	inline const FName Key_Observation_Self_Dynamic_ActiveWeapon_Type = FName("Observation.Self.Dynamic.ActiveWeapon.Type");
	inline const FName Key_Observation_Self_Dynamic_ActiveWeapon_Mastery = FName("Observation.Self.Dynamic.ActiveWeapon.Mastery");
	inline const FName Key_Observation_Self_Dynamic_ActiveWeapon_PowerRate = FName("Observation.Self.Dynamic.ActiveWeapon.PowerRate");

	inline const FName Key_Observation_Self_Static = FName("Observation.Self.Static");
	inline const FName Key_Observation_Self_Static_Level = FName("Observation.Self.Static.Level");
	inline const FName Key_Observation_Self_Static_ArmorRate = FName("Observation.Self.Static.ArmorRate");
	inline const FName Key_Observation_Self_Static_SurvivalDesire = FName("Observation.Self.Static.SurvivalDesire");

	inline const FName Key_Observation_Enemies = FName("Observation.Enemy.Array");
	inline const FName Key_Observation_Enemy = FName("Observation.Enemy");
	inline const FName Key_Observation_Enemy_Dynamic = FName("Observation.Enemy.Dynamic");
	inline const FName Key_Observation_Enemy_Dynamic_Observations = FName("Observation.Enemy.Dynamic.Observations");
	inline const FName Key_Observation_Enemy_Static = FName("Observation.Enemy.Static");
	inline const FName Key_Observation_Enemy_Static_Observations = FName("Observation.Enemy.Static.Observations");

	inline const FName Key_Observation_Enemy_Dynamic_Health = FName("Observation.Enemy.Dynamic.Health");
	inline const FName Key_Observation_Enemy_Dynamic_IsAlive = FName("Observation.Enemy.Dynamic.IsAlive");
	inline const FName Key_Observation_Enemy_Dynamic_Velocity = FName("Observation.Enemy.Dynamic.Velocity");
	inline const FName Key_Observation_Enemy_Dynamic_Location = FName("Observation.Enemy.Dynamic.Location");
	inline const FName Key_Observation_Enemy_Dynamic_Orientation = FName("Observation.Enemy.Dynamic.Orientation");
	inline const FName Key_Observation_Enemy_Dynamic_AgentCanSeeEnemy = FName("Observation.Enemy.Dynamic.AgentCanSeeEnemy");
	inline const FName Key_Observation_Enemy_Dynamic_EnemyCanSeeAgent = FName("Observation.Enemy.Dynamic.EnemyCanSeeAgent");
	inline const FName Key_Observation_Enemy_Dynamic_KillDesire = FName("Observation.Enemy.Dynamic.KillDesire");
	inline const FName Key_Observation_Enemy_Dynamic_Gesture = FName("Observation.Enemy.Dynamic.Gesture");
	inline const FName Key_Observation_Enemy_Dynamic_Gesture_Optional = FName("Observation.Enemy.Dynamic.Gesture.Optional");
	inline const FName Key_Observation_Enemy_Dynamic_Phrase = FName("Observation.Enemy.Dynamic.Phrase");
	inline const FName Key_Observation_Enemy_Dynamic_Phrase_Optional = FName("Observation.Enemy.Dynamic.Phrase.Optional");
	inline const FName Key_Observation_Enemy_Dynamic_CombatState = FName("Observation.Enemy.Dynamic.CombatState");
	inline const FName Key_Observation_Enemy_Dynamic_Weapon = FName("Observation.Enemy.Dynamic.Weapon");

	inline const FName Key_Observation_Enemy_Static_ArmorRate = FName("Observation.Enemy.Static.ArmorRate");
	inline const FName Key_Observation_Enemy_Static_Level = FName("Observation.Enemy.Static.Level");

	inline const FName Key_Observation_Combat_State_Duration = FName("Observation.Combat.State.Duration");
	inline const FName Key_Observation_Combat_State_EnemiesCount = FName("Observation.Combat.State.EnemiesCount");
	inline const FName Key_Observation_Combat_State_AlliesCount = FName("Observation.Combat.State.AlliesCount");
	inline const FName Key_Observation_Ally = FName("Observation.Ally");
	inline const FName Key_Observation_Ally_Dynamic = FName("Observation.Ally.Dynamic");
	inline const FName Key_Observation_Ally_Dynamic_IsAlive = FName("Observation.Ally.Dynamic.IsAlive");
	inline const FName Key_Observation_Ally_Dynamic_Health = FName("Observation.Ally.Dynamic.Health");
	inline const FName Key_Observation_Ally_Dynamic_Velocity = FName("Observation.Ally.Dynamic.Velocity");
	inline const FName Key_Observation_Ally_Dynamic_Location = FName("Observation.Ally.Dynamic.Location");
	inline const FName Key_Observation_Ally_Dynamic_Orientation = FName("Observation.Ally.Dynamic.Orientation");
	inline const FName Key_Observation_Ally_Dynamic_Gesture = FName("Observation.Ally.Dynamic.Gesture");
	inline const FName Key_Observation_Ally_Dynamic_Gesture_Optional = FName("Observation.Ally.Dynamic.Gesture.Optional");
	inline const FName Key_Observation_Ally_Dynamic_Phrase = FName("Observation.Ally.Dynamic.Phrase");
	inline const FName Key_Observation_Ally_Dynamic_Phrase_Optional = FName("Observation.Ally.Dynamic.Phrase.Optional");
	inline const FName Key_Observation_Ally_Dynamic_CombatState = FName("Observation.Ally.Dynamic.CombatState");
	inline const FName Key_Observation_Ally_Dynamic_AgentCanSeeAlly = FName("Observation.Ally.Dynamic.AgentCanSeeAlly");
	inline const FName Key_Observation_Ally_Dynamic_Weapon = FName("Observation.Ally.Dynamic.Weapon");
	inline const FName Key_Observation_Ally_Dynamic_Observations = FName("Observation.Ally.Dynamic.Observations");
	inline const FName Key_Observation_Ally_Static = FName("Observation.Ally.Static");
	inline const FName Key_Observation_Ally_Static_Observations = FName("Observation.Ally.Static.Observations");
	inline const FName Key_Observation_Ally_Static_Level = FName("Observation.Ally.Static.Level");
	inline const FName Key_Observation_Ally_Static_ArmorRate = FName("Observation.Ally.Static.ArmorRate");
}

namespace LAActionKeys
{
	inline const FName Key_Action_All = FName("Action.All");
	
	inline const FName Key_Action_Locomotion_SetSpeed = FName("Action.Locomotion.SetSpeed");
	inline const FName Key_Action_Locomotion_Move = FName("Action.Locomotion.Move");
	inline const FName Key_Action_Locomotion_Rotate = FName("Action.Locomotion.Rotate");
	inline const FName Key_Action_Locomotion_Jump = FName("Action.Locomotion.Jump");
	inline const FName Key_Action_Locomotion_Climb = FName("Action.Locomotion.Climb");
	inline const FName Key_Action_Locomotion_NonBlocking = FName("Action.Locomotion.NonBlocking");
	inline const FName Key_Actions_Locomotion = FName("Action.Locomotion");
	
	// these are like "actions that can be done inclusively with locomotion, but not locomotion themselves"
	inline const FName Key_Action_Gesture = FName("Action.Gesture");
	inline const FName Key_Action_SayPhrase = FName("Action.SayPhrase");
	inline const FName Key_Action_UseConsumableItem = FName("Action.UseConsumableItem");
	
	inline const FName Key_Action_Combat = FName("Action.Combat");
	inline const FName Key_Action_Combat_Attack = FName("Action.Combat.Attack");
	inline const FName Key_Action_Combat_Parry = FName("Action.Combat.Parry");
	inline const FName Key_Action_Combat_Dodge = FName("Action.Combat.Dodge");
	
	inline const FName Key_Action_Null = FName("Action.Null");
}

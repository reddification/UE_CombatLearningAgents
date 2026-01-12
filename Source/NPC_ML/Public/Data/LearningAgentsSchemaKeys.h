#pragma once

namespace LAObservationKeys
{
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

	inline const FName Key_Observation_Combat_State = FName("Observation.Combat.State");

	inline const FName Key_Observation_Dynamic = FName("Observation.Dynamic");
	inline const FName Key_Observation_Dynamic_Health = FName("Observation.Dynamic.Health");
	inline const FName Key_Observation_Dynamic_Stamina = FName("Observation.Dynamic.Stamina");
	inline const FName Key_Observation_Dynamic_Velocity = FName("Observation.Dynamic.Velocity");
	inline const FName Key_Observation_Dynamic_CombatStates = FName("Observation.Dynamic.CombatStates");
	
	inline const FName Key_Observation_Dynamic_Gesture = FName("Observation.Self.Dynamic.Gesture.");
	inline const FName Key_Observation_Dynamic_Gesture_Optional = FName("Observation.Self.Dynamic.Gesture.Optional");
	inline const FName Key_Observation_Dynamic_Phrase = FName("Observation.Self.Dynamic.Phrase");
	inline const FName Key_Observation_Dynamic_Phrase_Optional = FName("Observation.Self.Dynamic.Phrase.Optional");
	
	inline const FName Key_Observation_ActiveWeapon = FName("Observation.Self.Dynamic.ActiveWeapon.Range");
	inline const FName Key_Observation_ActiveWeapon_Range = FName("Observation.Self.Dynamic.ActiveWeapon.Range");
	inline const FName Key_Observation_ActiveWeapon_Range_Melee = FName("Observation.Self.Dynamic.ActiveWeapon.Range.Melee");
	inline const FName Key_Observation_ActiveWeapon_Range_Ranged = FName("Observation.Self.Dynamic.ActiveWeapon.Range.Ranged");
	inline const FName Key_Observation_ActiveWeapon_Type = FName("Observation.Self.Dynamic.ActiveWeapon.Type");
	inline const FName Key_Observation_ActiveWeapon_Mastery = FName("Observation.Self.Dynamic.ActiveWeapon.Mastery");
	inline const FName Key_Observation_ActiveWeapon_PowerRate = FName("Observation.Self.Dynamic.ActiveWeapon.PowerRate");

	inline const FName Key_Observation_Static = FName("Observation.Self.Static");
	inline const FName Key_Observation_Static_Level = FName("Observation.Self.Static.Level");
	inline const FName Key_Observation_Static_ArmorRate = FName("Observation.Self.Static.ArmorRate");
	inline const FName Key_Observation_Static_SurvivalDesire = FName("Observation.Self.Static.SurvivalDesire");

	inline const FName Key_Observation_Enemies = FName("Observation.Enemy.Array");
	inline const FName Key_Observation_Enemy = FName("Observation.Enemy");
	
	inline const FName Key_Observation_Allies = FName("Observation.Ally.Array");
	inline const FName Key_Observation_Ally = FName("Observation.Ally");
	
	inline const FName Key_Observation_Actor_Dynamic_IsAlive = FName("Observation.Actor.Dynamic.IsAlive");
	inline const FName Key_Observation_Actor_Dynamic_AgentCanSeeActor = FName("Observation.Actor.Dynamic.AgentCanSeeEnemy");
	inline const FName Key_Observation_Actor_Dynamic_ActorCanSeeAgent = FName("Observation.Actor.Dynamic.EnemyCanSeeAgent");
	inline const FName Key_Observation_Actor_Dynamic_KillDesire = FName("Observation.Actor.Dynamic.KillDesire");

	inline const FName Key_Observation_Actor_Dynamic_RaindropTo_Lidar = FName("Observation.Actor.Dynamic.RaindropTo.Lidar");
	inline const FName Key_Observation_Actor_Dynamic_RaindropTo_Array = FName("Observation.Actor.Dynamic.RaindropTo.Array");
	inline const FName Key_Observation_Actor_Dynamic_RaindropTo_Convolved = FName("Observation.Actor.Dynamic.RaindropTo.Convolved");
	inline const FName Key_Observation_Actor_Dynamic_RaindropTo_Convolved_Optional = FName("Observation.Actor.Dynamic.RaindropTo.Convolved.Optional");
	
	inline const FName Key_Observation_Actor_Dynamic_Location = FName("Observation.Actor.Dynamic.Location");
	inline const FName Key_Observation_Actor_Dynamic_Orientation = FName("Observation.Actor.Dynamic.Orientation");
	inline const FName Key_Observation_Combat_State_Duration = FName("Observation.Combat.State.Duration");
	inline const FName Key_Observation_Combat_State_EnemiesCount = FName("Observation.Combat.State.EnemiesCount");
	inline const FName Key_Observation_Combat_State_AlliesCount = FName("Observation.Combat.State.AlliesCount");
	
	inline const FName Key_Observation_CombatHistory = FName("Observation.Ally.Static.Observations");
	inline const FName Key_Observation_CombatHistory_Array = FName("Observation.Combat.History.Array");
	inline const FName Key_Observation_CombatHistory_Event = FName("Observation.Combat.History.Event");
	inline const FName Key_Observation_CombatHistory_Event_Type = FName("Observation.Combat.History.Event.Type");
	inline const FName Key_Observation_CombatHistory_Event_Time = FName("Observation.Combat.History.Event.Time");
	
	inline const FName Key_Observation_TranslationHistory = FName("Observation.Ally.Static.Observations");
	inline const FName Key_Observation_TranslationHistory_Array = FName("Observation.TranslationHistory.Array");
	inline const FName Key_Observation_TranslationHistory_Entry = FName("Observation.TranslationHistory.Entry");
	inline const FName Key_Observation_TranslationHistory_Velocity = FName("Observation.TranslationHistory.Entry.Velocity");
	inline const FName Key_Observation_TranslationHistory_Entry_Location = FName("Observation.TranslationHistory.Entry.Location");
	inline const FName Key_Observation_TranslationHistory_Entry_Rotation = FName("Observation.TranslationHistory.Entry.Rotation");
}

namespace LAActionKeys
{
	inline const FName Key_Action_All = FName("Action.All");
	
	inline const FName Key_Action_Locomotion_SetSpeed = FName("Action.Locomotion.SetSpeed");
	inline const FName Key_Action_Locomotion_Move = FName("Action.Locomotion.Move");
	inline const FName Key_Action_Locomotion_Rotate = FName("Action.Locomotion.Rotate");
	inline const FName Key_Action_Locomotion_Blocking = FName("Action.Locomotion.Blocking");
	inline const FName Key_Action_Locomotion_NonBlocking = FName("Action.Locomotion.NonBlocking");
	inline const FName Key_Action_Locomotion_NonBlocking_Animation = FName("Action.Locomotion.NonBlocking.Animation");
	inline const FName Key_Action_Locomotion = FName("Action.Locomotion");
	
	// these are like "actions that can be done inclusively with locomotion, but not locomotion themselves"
	inline const FName Key_Action_Gesture = FName("Action.Gesture");
	inline const FName Key_Action_SayPhrase = FName("Action.SayPhrase");
	inline const FName Key_Action_UseConsumableItem = FName("Action.UseConsumableItem");
	inline const FName Key_Action_ChangeWeaponState = FName("Action.ChangeWeaponState");
	
	inline const FName Key_Action_Combat = FName("Action.Combat");
	inline const FName Key_Action_Combat_Attack = FName("Action.Combat.Attack");
	inline const FName Key_Action_Combat_Parry = FName("Action.Combat.Parry");
	inline const FName Key_Action_Combat_Dodge = FName("Action.Combat.Dodge");
	
	inline const FName Key_Action_Null = FName("Action.Null");
}

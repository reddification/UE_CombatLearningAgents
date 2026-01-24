#include "LearningAgentsTags_Combat.h"

namespace LearningAgentsTags_Combat
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Observation_Self, "LearningAgents.Combat.Observation.Self", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Observation_Enemy, "LearningAgents.Combat.Observation.Enemy", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Observation_Ally, "LearningAgents.Combat.Observation.Ally", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Observation_CombatState, "LearningAgents.Combat.Observation.Combat.State", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Observation_Surrounding, "LearningAgents.Combat.Observation.Surrounding", "");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Behavior_Combat_RL, "LearningAgents.Behavior.Combat.RL", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Behavior_Combat_IL_Recording, "LearningAgents.Behavior.Combat.IL.Recording", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Behavior_Combat_IL_Training, "LearningAgents.Behavior.Combat.IL.Training", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Behavior_Combat_Inference, "LearningAgents.Behavior.Combat.Inference", "");
}
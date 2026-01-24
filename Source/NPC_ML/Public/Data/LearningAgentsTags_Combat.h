#pragma once
#include "NativeGameplayTags.h"

namespace LearningAgentsTags_Combat
{
	NPC_ML_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Observation_Self);
	NPC_ML_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Observation_Enemy);
	NPC_ML_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Observation_Ally);
	NPC_ML_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Observation_CombatState);
	NPC_ML_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Observation_Surrounding);
	
	NPC_ML_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Behavior_Combat_RL);
	NPC_ML_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Behavior_Combat_IL_Recording);
	NPC_ML_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Behavior_Combat_IL_Training);
	NPC_ML_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Behavior_Combat_Inference);
}

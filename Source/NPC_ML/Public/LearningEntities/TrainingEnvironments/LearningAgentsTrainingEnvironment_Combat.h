// 

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsTraining/Public/LearningAgentsTrainingEnvironment.h"
#include "LearningAgentsTrainingEnvironment_Combat.generated.h"

/**
 * 
 */
UCLASS()
class NPC_ML_API ULearningAgentsTrainingEnvironment_Combat : public ULearningAgentsTrainingEnvironment
{
	GENERATED_BODY()

public:
	virtual void GatherAgentReward_Implementation(float& OutReward, const int32 AgentId) override;
	virtual void GatherAgentCompletion_Implementation(ELearningAgentsCompletion& OutCompletion, const int32 AgentId) override;
	virtual void ResetAgentEpisode_Implementation(const int32 AgentId) override;
};

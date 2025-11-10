// 


#include "LearningEntities/TrainingEnvironments/LearningAgentsTrainingEnvironment_Combat.h"

#include "LearningAgentsRewards.h"
#include "Interfaces/LearningAgentObservation.h"

void ULearningAgentsTrainingEnvironment_Combat::GatherAgentReward_Implementation(float& OutReward, const int32 AgentId)
{
	Super::GatherAgentReward_Implementation(OutReward, AgentId);
	// ULearningAgentsRewards::MakeRewardOnCondition()
}

void ULearningAgentsTrainingEnvironment_Combat::GatherAgentCompletion_Implementation(
	ELearningAgentsCompletion& OutCompletion, const int32 AgentId)
{
	Super::GatherAgentCompletion_Implementation(OutCompletion, AgentId);
	OutCompletion = ELearningAgentsCompletion::Running;
	auto Agent = Cast<ILearningAgentObservation>(GetAgent(AgentId));
	OutCompletion = ULearningAgentsCompletions::MakeCompletionOnCondition(Agent->IsDead(), ELearningAgentsCompletion::Termination, FName("Completion.Combat.Dead.Self"));
	
	// ULearningAgentsCompletions::MakeCompletionOnCondition()
}

void ULearningAgentsTrainingEnvironment_Combat::ResetAgentEpisode_Implementation(const int32 AgentId)
{
	Super::ResetAgentEpisode_Implementation(AgentId);
}

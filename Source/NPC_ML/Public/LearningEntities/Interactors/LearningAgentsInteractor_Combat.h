// 

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsInteractor.h"
#include "LearningAgentsInteractor_Combat.generated.h"

/**
 * 
 */
UCLASS()
class NPC_ML_API ULearningAgentsInteractor_Combat : public ULearningAgentsInteractor
{
	GENERATED_BODY()

public:
	virtual void SpecifyAgentObservation_Implementation(FLearningAgentsObservationSchemaElement& OutObservationSchemaElement, ULearningAgentsObservationSchema* InObservationSchema) override;
	virtual void SpecifyAgentAction_Implementation(FLearningAgentsActionSchemaElement& OutActionSchemaElement, ULearningAgentsActionSchema* InActionSchema) override;
	virtual void GatherAgentObservation_Implementation(FLearningAgentsObservationObjectElement& OutObservationObjectElement, ULearningAgentsObservationObject* InObservationObject, const int32 AgentId) override;
	virtual void PerformAgentAction_Implementation(const ULearningAgentsActionObject* InActionObject, const FLearningAgentsActionObjectElement& InActionObjectElement, const int32 AgentId) override;

private:
	TMap<FName, FLearningAgentsObservationSchemaElement> GetSelfObservations(ULearningAgentsObservationSchema* InObservationSchema) const;
	TMap<FName, FLearningAgentsObservationSchemaElement> GetLidarObservations(ULearningAgentsObservationSchema* InObservationSchema) const;
	TMap<FName, FLearningAgentsObservationSchemaElement> GetEnemiesObservations(ULearningAgentsObservationSchema* InObservationSchema) const;
	TMap<FName, FLearningAgentsObservationSchemaElement> GetAlliesObservations(ULearningAgentsObservationSchema* InObservationSchema) const;
	TMap<FName, FLearningAgentsObservationSchemaElement> GetEnvironmentObservations(ULearningAgentsObservationSchema* InObservationSchema) const;
};

// 

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_MLTrainingEpisodeLocation.generated.h"

/**
 * 
 */
UCLASS()
class NPC_ML_API UEnvQueryContext_MLTrainingEpisodeLocation : public UEnvQueryContext
{
	GENERATED_BODY()
	
public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};

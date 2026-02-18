#include "EQS/Contexts/EnvQueryContext_MLTrainingEpisodeLocation.h"

#include "Components/TrainingEpisodeSetupComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"

void UEnvQueryContext_MLTrainingEpisodeLocation::ProvideContext(FEnvQueryInstance& QueryInstance,
                                                                FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);
	auto OwnerTESComponent = Cast<UTrainingEpisodeSetupComponent>(QueryInstance.Owner.Get());
	if (OwnerTESComponent != nullptr)
	{
		const FVector& EpisodeOriginLocation = OwnerTESComponent->GetEpisodeOriginLocation();
		if (EpisodeOriginLocation != FVector::ZeroVector)
			UEnvQueryItemType_Point::SetContextHelper(ContextData, EpisodeOriginLocation);
	}
}

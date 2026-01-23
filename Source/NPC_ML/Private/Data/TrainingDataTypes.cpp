#include "Data/TrainingDataTypes.h"

#include "Kismet/GameplayStatics.h"

bool FMLTrainingEpisodeActorSetupAction_FinishDeferredSpawn::SetupInternal(AActor* Actor, FExternalMemory* Memory) const
{
	Super::SetupInternal(Actor, Memory);
	auto SpawnedCharacter = UGameplayStatics::FinishSpawningActor(Actor, Actor->GetTransform());
	return IsValid(SpawnedCharacter);
}

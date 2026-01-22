#include "Data/TrainingDataTypes.h"

#include "Kismet/GameplayStatics.h"

bool FMLTrainingEpisodeCharacterSetupAction_FinishDeferredSpawn::SetupInternal(APawn* Character, FExternalMemory* Memory) const
{
	Super::SetupInternal(Character, Memory);
	auto SpawnedCharacter = UGameplayStatics::FinishSpawningActor(Character, Character->GetTransform());
	return IsValid(SpawnedCharacter);
}

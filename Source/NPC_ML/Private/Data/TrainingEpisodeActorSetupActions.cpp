#include "Data/TrainingEpisodeActorSetupActions.h"
#include "Kismet/GameplayStatics.h"

bool FMLTrainingEpisodeActorSetupAction_Base::Setup(AActor* Actor, FExternalMemoryPtr Memory) const
{
	if (CanSetup(Actor))
	{
		if (Memory != nullptr)
			Memory->bCanSetup = true;
				
		return SetupInternal(Actor, Memory);
	}
		
	return false;
}

bool FMLTrainingEpisodeActorSetupAction_Base::Repeat(AActor* Actor, FExternalMemoryPtr Memory) const
{
	if (Memory != nullptr && Memory->bCanSetup)
		return RepeatInternal(Actor, Memory);
		
	return false;
}

float FMLTrainingEpisodeActorSetupAction_Base::CanSetup(AActor* Actor) const
{
	return bActive && (SetupChance >= 1.f ? true : FMath::RandRange(0.f, 1.f) <= SetupChance);
}

bool FMLTrainingEpisodeActorSetupAction_FinishDeferredSpawn::SetupInternal(AActor* Actor, FExternalMemoryPtr Memory) const
{
	Super::SetupInternal(Actor, Memory);
	auto SpawnedCharacter = UGameplayStatics::FinishSpawningActor(Actor, Actor->GetTransform());
	return IsValid(SpawnedCharacter);
}
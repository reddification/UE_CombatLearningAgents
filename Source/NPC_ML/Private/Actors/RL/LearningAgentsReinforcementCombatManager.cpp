#include "Actors/RL/LearningAgentsReinforcementCombatManager.h"

#include "Components/CombatLearningAgentsManagerComponent.h"

ALearningAgentsReinforcementCombatManager::ALearningAgentsReinforcementCombatManager()
{
	PrimaryActorTick.bCanEverTick = true;
	CombatLearningAgentManagerComponent = CreateDefaultSubobject<UCombatLearningAgentsManagerComponent>(TEXT("Learning Agent Manager Component"));
}

void ALearningAgentsReinforcementCombatManager::BeginPlay()
{
	Super::BeginPlay();
}

void ALearningAgentsReinforcementCombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


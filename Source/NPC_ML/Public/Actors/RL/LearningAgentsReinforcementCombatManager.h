// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LearningAgentsReinforcementCombatManager.generated.h"

class UCombatLearningAgentsManagerComponent;

UCLASS()
class NPC_ML_API ALearningAgentsReinforcementCombatManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALearningAgentsReinforcementCombatManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCombatLearningAgentsManagerComponent* CombatLearningAgentManagerComponent;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

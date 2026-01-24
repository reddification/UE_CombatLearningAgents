// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "MLManagerBase.generated.h"

class ULearningAgentsManager;

UCLASS()
class NPC_ML_API AMLManagerBase : public AActor
{
	GENERATED_BODY()

public:
	AMLManagerBase();
	
	ULearningAgentsManager* GetLearningAgentsManager() const { return LearningAgentsManager; };
	void SetManagerActive(bool bNewActive);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ULearningAgentsManager> LearningAgentsManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag BehaviorTag;

	bool bActive = false;
};

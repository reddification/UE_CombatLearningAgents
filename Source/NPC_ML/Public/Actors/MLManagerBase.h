// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "MLManagerBase.generated.h"

class ULearningAgentsInteractor_Base;
class ULearningAgentsInteractor;
class ULearningAgentsManager;

UCLASS()
class NPC_ML_API AMLManagerBase : public AActor
{
	GENERATED_BODY()

public:
	AMLManagerBase();
	
	ULearningAgentsManager* GetLearningAgentsManager() const { return LearningAgentsManager; };
	virtual void SetManagerActive(bool bNewActive);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ULearningAgentsManager> LearningAgentsManager;

	UPROPERTY()
	TObjectPtr<ULearningAgentsInteractor_Base> Interactor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag BehaviorTag;

	bool bActive = false;
};

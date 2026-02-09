// 

#pragma once

#include "CoreMinimal.h"
#include "Data/TrainingDataTypes.h"
#include "GameFramework/Actor.h"
#include "TrainingEpisodePCG.generated.h"

class UPCGGraph;
class UBoxComponent;
class UPCGComponent;

UCLASS()
class NPC_ML_API ATrainingEpisodePCG : public AActor
{
	GENERATED_BODY()

private:
	DECLARE_MULTICAST_DELEGATE(FTrainingEpisodePcgEvent);
	
public:
	ATrainingEpisodePCG();
	
	void SetPCG(class UPCGGraph* NewPcgGraph, bool bGenerateImmediately);
	bool Generate();
	bool Cleanup();

	bool CleanupAndDestroy();
	bool IsReady() const;
	void SetSeed(int32 NewSeed, const FName& EpisodeSeedParameterName);
	void SetParameters(const FPcgParametersContainer& Parameters);
	void SetExtents(const FVector& Extents);

	mutable FTrainingEpisodePcgEvent CleanupCompletedEvent;
	mutable FTrainingEpisodePcgEvent GenerationCompletedEvent;
	mutable FTrainingEpisodePcgEvent NewGraphSetEvent;

#if WITH_EDITOR
	void StopGenerationInProgress();
#endif
	
protected:
	virtual void PostInitializeComponents() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> BoundsComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPCGComponent> PCGComponent;
	
	virtual void OnPCGCleanupCompleted(UPCGComponent* PcgComponent);
	virtual void OnPCGGenerateCompleted(UPCGComponent* PcgComponent);
	
private:
	bool bPendingDestroy = false;
	bool bPendingGraphChange = false;
	bool bGenerateGraphWhenChanged =false;
	
	UPROPERTY()
	TObjectPtr<UPCGGraph> PendingGraphReplacement;
};

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MLTrainingEpisodesPreset.generated.h"

struct FMLTrainingEpisodeTemplate;
/**
 * 
 */
UCLASS(Blueprintable)
class NPC_ML_API UMLTrainingEpisodesPreset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FMLTrainingEpisodeTemplate> TrainingPresets;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUseRandomEpisodeSetup = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bRandomizeSeedOnSetup = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName EpisodeSeedParameterName = FName("EpisodeSeed");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EpisodeSeed = 12345;
	
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};

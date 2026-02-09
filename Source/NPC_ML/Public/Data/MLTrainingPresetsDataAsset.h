#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MLTrainingPresetsDataAsset.generated.h"

struct FMLTrainingPreset;
/**
 * 
 */
UCLASS()
class NPC_ML_API UMLTrainingPresetsDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FMLTrainingPreset> TrainingPresets;
	
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

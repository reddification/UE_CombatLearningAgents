// 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MLTrainingEditorConfig.generated.h"

class UMLTrainingEpisodesPreset;
class UMLTrainingConfigurationBase;
class AMLTrainingManager;

USTRUCT(BlueprintType)
struct FMLEditorConfigPreset
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UserDescription;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<AMLTrainingManager> TrainingManagerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UMLTrainingConfigurationBase> TrainingConfiguration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UMLTrainingEpisodesPreset> TrainingEpisodesPreset;
};

/**
 * 
 */
UCLASS(BlueprintType)
class NPC_ML_API UMLTrainingEditorConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FMLEditorConfigPreset> Presets;
};

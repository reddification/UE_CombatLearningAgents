#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsObservations.h"
#include "Data/RaindropDataTypes.h"
#include "Engine/DeveloperSettings.h"
#include "RaindropSettings.generated.h"

struct FRaindropParams;

USTRUCT(BlueprintType)
struct FRaindropPreset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString UserDescription;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bEnabled = true;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition=bEnabled))
	FName ObservationKey;
	
	// request raindrop each UpdateInterval seconds;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition=bEnabled))
	float UpdateInterval = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition=bEnabled))
	TArray<FRaindropGridDescriptor> Grids;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition=bEnabled))
	FRaindropParams Params;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition=bEnabled))
	FConv2dObservationParams Conv2dParams;
	
	FORCEINLINE bool IsValid() const { return bEnabled && !Grids.IsEmpty(); };
};

/**
 * 
 */
UCLASS(Config=Game, defaultconfig, DisplayName="Raindrop")
class NPC_ML_API URaindropSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config)
	TArray<FRaindropPreset> Configs;
};

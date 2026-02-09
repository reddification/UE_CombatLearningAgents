// 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MLTrainingManagerPcgOwnerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UMLTrainingManagerPcgOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NPC_ML_API IMLTrainingManagerPcgOwnerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void SetTrainingEpisodeOrigin(const FVector& Origin);
	virtual void SetTrainingEpisodeOrigin_Implementation(const FVector& Origin) { };
};

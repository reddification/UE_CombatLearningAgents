// 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LearningAgentObservation.generated.h"

// This class does not need to be modified.
UINTERFACE()
class ULearningAgentObservation : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NPC_ML_API ILearningAgentObservation
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FVector GetSelfLocation() const = 0;
	virtual FVector GetActiveTargetLocation() const = 0;

	virtual TArray<FVector> GetObservedFloors() const = 0;
	virtual TArray<FVector> GetObservedCeilings() const = 0;
	virtual TArray<FVector> GetObservedWalls() const = 0;
	virtual TArray<FVector> GetObservedDoors() const = 0;
	virtual TArray<FVector> GetObservedWindows() const = 0;
	virtual TArray<FVector> GetObservedLowClimbableLedges() const = 0;
	virtual TArray<FVector> GetObservedHighClimbableLedges() const = 0;
	virtual bool IsDead() const = 0;;
};

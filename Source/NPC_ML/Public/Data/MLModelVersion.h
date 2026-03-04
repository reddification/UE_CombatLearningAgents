// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LearningAgentsCritic.h"
#include "LearningAgentsPolicy.h"
#include "Engine/DataAsset.h"
#include "MLModelVersion.generated.h"

class ULearningAgentsInteractor_Base;
class ULearningAgentsRecording;
class ULearningAgentsNeuralNetwork;
class ULearningAgentsInteractor;
/**
 * 
 */
UCLASS()
class NPC_ML_API UMLModelVersion : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Info")
	FString Name;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Info")
	FString UserDescription;	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Policy")
	TSubclassOf<ULearningAgentsPolicy> PolicyClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Policy")
	FLearningAgentsPolicySettings PolicySettings;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Critic")
	TSubclassOf<ULearningAgentsCritic> CriticClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Critic")
	FLearningAgentsCriticSettings CriticSettings;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weights")
	ULearningAgentsNeuralNetwork* EncoderNN;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weights")
	ULearningAgentsNeuralNetwork* PolicyNN;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weights")
	ULearningAgentsNeuralNetwork* DecoderNN;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weights")
	TSoftObjectPtr<ULearningAgentsNeuralNetwork> CriticNN;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Imitation Learning")
	TSoftObjectPtr<ULearningAgentsRecording> RecordingAsset;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interactor")
	TSubclassOf<ULearningAgentsInteractor_Base> InteractorClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interactor")
	FGameplayTagContainer RelevantObservations;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interactor")
	FGameplayTagContainer RelevantActions;
	
public:
	ULearningAgentsInteractor_Base* MakeInteractor(ULearningAgentsManager* LAM) const;
	
	bool IsSet() const;
	
	UFUNCTION(CallInEditor)
	void Reset();
};

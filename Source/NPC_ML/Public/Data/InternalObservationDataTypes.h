#pragma once

#include "RaindropDataTypes.h"
#include "Data/AgentCombatDataTypes.h"

struct FOtherCharacterObservationData
{
	virtual ~FOtherCharacterObservationData() = default;
	FOtherCharacterObservationData() : CharacterState(MakeShared<CombatLearning::FOtherCharacterState>()) {  }
	
	FRaindrop RaindropData;
	FVector LastKnownLocation = FVector::ZeroVector;
	TSharedRef<CombatLearning::FOtherCharacterState> CharacterState;
};
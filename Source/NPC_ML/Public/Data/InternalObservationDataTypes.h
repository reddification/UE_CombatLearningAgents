#pragma once

#include "RaindropDataTypes.h"
#include "Data/AgentCombatDataTypes.h"

struct FOtherCharacterObservationData
{
	virtual ~FOtherCharacterObservationData() = default;
	FOtherCharacterObservationData() : CharacterState(MakeShared<CombatLearning::FPerceivedCharacterData>()) {  }
	
	FRaindrop RaindropData;
	FVector LastKnownLocation = FVector::ZeroVector;
	TSharedRef<CombatLearning::FPerceivedCharacterData> CharacterState;
};
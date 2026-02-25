#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ILAction_Locomotion_NonBlocking.h"
#include "Data/LearningAgentsSchemaKeys.h"

namespace LearningAgentsImitationActions
{
	class FAction_Phrase : public FAction_Locomotion_NonBlocking
	{
	private:
		using Super = FAction_Locomotion_NonBlocking;
		
	public:
		FAction_Phrase(float Timestamp, const FGameplayTag& InPhraseTag) : Super(Timestamp), PhraseTag(InPhraseTag)
		{
			SetType(LAActionKeys::Key_Action_SayPhrase);
		}

		virtual void Combine(const TSharedPtr<FAction>& OtherAction) override;
		virtual FName GetActionName() const override { return LAActionKeys::Key_Action_SayPhrase; };
		virtual FLearningAgentsActionObjectElement GetActionInternal(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
					
	private:
		FGameplayTag PhraseTag;
	};
}

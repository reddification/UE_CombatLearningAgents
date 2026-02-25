#include "Data/ILActions/ILAction_Phrase.h"

namespace LearningAgentsImitationActions
{
	void FAction_Phrase::Combine(const TSharedPtr<FAction>& OtherAction)
	{
		Super::Combine(OtherAction);
		if (IsTypeOf(OtherAction->GetType()))
		{
			PhraseTag = StaticCastSharedPtr<FAction_Phrase>(OtherAction)->PhraseTag;
		}
	}

	FLearningAgentsActionObjectElement FAction_Phrase::GetActionInternal(
		ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const
	{
		return ULearningAgentsActions::MakeNamedExclusiveDiscreteAction(InActionObject, PhraseTag.GetTagName(), LAActionKeys::Key_Action_SayPhrase);
	}
}
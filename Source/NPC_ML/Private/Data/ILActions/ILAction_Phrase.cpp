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
		auto Action = ULearningAgentsActions::MakeNamedExclusiveDiscreteAction(InActionObject, PhraseTag.GetTagName(), LAActionKeys::Key_Action_SayPhrase);
		auto ActionOptional = ULearningAgentsActions::MakeOptionalValidAction(InActionObject, Action, LAActionKeys::Key_Action_Locomotion_NonBlocking_OptionalElement);
		return ActionOptional;
	}
}
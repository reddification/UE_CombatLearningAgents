// 


#include "Data/ILActions/ILAction_Locomotion_BlockingAction.h"

namespace LearningAgentsImitationActions
{
	FLearningAgentsActionObjectElement FAction_Locomotion_BlockingLocomotion::GetAction(
		ULearningAgentsActionObject* InActionObject, AActor* AgentActor, ULearningAgentsInteractor_Combat* Interactor) const
	{
		auto Action = ULearningAgentsActions::MakeEnumAction(InActionObject, StaticEnum<ELALocomotionAction>(),
			static_cast<uint8>(LocomotionAction), GetActionName());
		auto LocomotionExclusiveUnionAction = ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, GetActionName(), 
			Action, LAActionKeys::Key_Action_Locomotion);
		
		return ULearningAgentsActions::MakeExclusiveUnionAction(InActionObject, LAActionKeys::Key_Action_Locomotion, LocomotionExclusiveUnionAction,
		                                                        LAActionKeys::Key_Action_All);
	}
}
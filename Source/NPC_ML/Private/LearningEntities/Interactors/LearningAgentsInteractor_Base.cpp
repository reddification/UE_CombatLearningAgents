#include "LearningEntities/Interactors/LearningAgentsInteractor_Base.h"

#include "LearningAgentsManager.h"
#include "Data/LogChannels.h"

ULearningAgentsInteractor_Base* ULearningAgentsInteractor_Base::MakeUninitializedInteractor(ULearningAgentsManager* InManager,
	const TSubclassOf<ULearningAgentsInteractor_Base>& Class, const FName& Name)
{
	if (!InManager)
	{
		UE_LOG(LogNpcMl, Error, TEXT("MakeInteractor: InManager is nullptr."));
		return nullptr;
	}

	if (!Class)
	{
		UE_LOG(LogNpcMl, Error, TEXT("MakeInteractor: Class is nullptr."));
		return nullptr;
	}

	const FName UniqueName = MakeUniqueObjectName(InManager, Class, Name, EUniqueObjectNameOptions::GloballyUnique);

	return NewObject<ULearningAgentsInteractor_Base>(InManager, Class, UniqueName);
}

void ULearningAgentsInteractor_Base::SpecifyAgentObservation_Implementation(
	FLearningAgentsObservationSchemaElement& OutObservationSchemaElement,
	ULearningAgentsObservationSchema* InObservationSchema)
{
	// 1 Mar 2026 (aki): calling Super produces an error in logs which is wrong IMO. It interferes with my debugging process so I'm commenting it until plugin works properly
	// TODO uncomment super ASAP
	// Super::SpecifyAgentObservation_Implementation(OutObservationSchemaElement, InObservationSchema);
}

void ULearningAgentsInteractor_Base::SpecifyAgentAction_Implementation(
	FLearningAgentsActionSchemaElement& OutActionSchemaElement, ULearningAgentsActionSchema* InActionSchema)
{
	// 1 Mar 2026 (aki): calling Super produces an error in logs which is wrong IMO. It interferes with my debugging process so I'm commenting it until plugin works properly
	// TODO uncomment super ASAP
	// Super::SpecifyAgentAction_Implementation(OutActionSchemaElement, InActionSchema);
}

void ULearningAgentsInteractor_Base::GatherAgentObservation_Implementation(
	FLearningAgentsObservationObjectElement& OutObservationObjectElement,
	ULearningAgentsObservationObject* InObservationObject, const int32 AgentId)
{
	// 1 Mar 2026 (aki): calling Super produces an error in logs which is wrong IMO. It interferes with my debugging process so I'm commenting it until plugin works properly
	// TODO uncomment super ASAP
	// Super::GatherAgentObservation_Implementation(OutObservationObjectElement, InObservationObject, AgentId);
}

void ULearningAgentsInteractor_Base::PerformAgentAction_Implementation(
	const ULearningAgentsActionObject* InActionObject, const FLearningAgentsActionObjectElement& InActionObjectElement,
	const int32 AgentId)
{
	// 1 Mar 2026 (aki): calling Super produces an error in logs which is wrong IMO. It interferes with my debugging process so I'm commenting it until plugin works properly
	// TODO uncomment super ASAP
	// Super::PerformAgentAction_Implementation(InActionObject, InActionObjectElement, AgentId);
}

void ULearningAgentsInteractor_Base::MakeAgentActionModifier_Implementation(
	FLearningAgentsActionModifierElement& OutActionModifierElement, ULearningAgentsActionModifier* InActionModifier,
	const ULearningAgentsObservationObject* InObservationObject,
	const FLearningAgentsObservationObjectElement& InObservationObjectElement, const int32 AgentId)
{
	// 1 Mar 2026 (aki): calling Super produces an error in logs which is wrong IMO. It interferes with my debugging process so I'm commenting it until plugin works properly
	// TODO uncomment super ASAP
	// Super::MakeAgentActionModifier_Implementation(OutActionModifierElement, InActionModifier, InObservationObject,
	//                                               InObservationObjectElement, AgentId);
}

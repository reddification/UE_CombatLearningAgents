#include "Data/MLModelVersion.h"
#include "LearningAgentsInteractor.h"
#include "LearningAgentsRecording.h"
#include "LearningEntities/Interactors/LearningAgentsInteractor_Combat.h"

bool UMLModelVersion::IsSet() const
{
	return IsValid(InteractorClass) && IsValid(PolicyClass) 
		&& IsValid(EncoderNN) && IsValid(DecoderNN) && IsValid(PolicyNN);
}

void UMLModelVersion::Reset()
{
	if (IsValid(EncoderNN))
		EncoderNN->ResetNetwork();
	
	if (IsValid(PolicyNN))
		PolicyNN->ResetNetwork();
	
	if (IsValid(DecoderNN))
		DecoderNN->ResetNetwork();
	
	if (!CriticNN.IsNull())
	{
		auto RealCriticNN = CriticNN.LoadSynchronous();
		RealCriticNN->ResetNetwork();
	}
	
	if (!RecordingAsset.IsNull())
	{
		auto RealRecordingAsset = RecordingAsset.LoadSynchronous();
		RealRecordingAsset->ResetRecording();
	}
}

ULearningAgentsInteractor_Base* UMLModelVersion::MakeInteractor(ULearningAgentsManager* LAM) const
{
	if (!IsValid(InteractorClass))
		return nullptr;
	
	auto Interactor = ULearningAgentsInteractor_Base::MakeUninitializedInteractor(LAM, InteractorClass, FName("Interactor"));
	if (ensure(Interactor))
	{
		Interactor->SetRelevantObservations(RelevantObservations);
		Interactor->SetRelevantActions(RelevantActions);
		Interactor->SetupInteractor(LAM);
	}
	
	return Interactor && Interactor->IsSetup() ? Interactor : nullptr;
}

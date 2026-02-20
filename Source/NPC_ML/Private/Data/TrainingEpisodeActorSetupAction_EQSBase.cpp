#include "Data/TrainingEpisodeActorSetupAction_EQSBase.h"
#include "AITypes.h"
#include "EnvironmentQuery/EnvQuery.h"

FExternalMemoryPtr FCharacterInitializationSetupAction_RunEQS_Base::MakeMemory() const
{
	return MakeShared<FExternalMemory_EQS>();
}

float FCharacterInitializationSetupAction_RunEQS_Base::CanSetup(AActor* Actor) const
{
	return Super::CanSetup(Actor) && IsValid(EQSRequest.QueryTemplate);
}

bool FCharacterInitializationSetupAction_RunEQS_Base::SetupInternal(AActor* Actor,
                                                                    FExternalMemoryPtr ExternalMemory) const
{
	auto Base = Super::SetupInternal(Actor, ExternalMemory);
	if (!Base) return false;
	
	TSharedPtr<FExternalMemory_EQS> EQSMemory = StaticCastSharedPtr<FExternalMemory_EQS>(ExternalMemory);
	EQSMemory->QueryFinishedDelegate = FQueryFinishedSignature::CreateSP(EQSMemory.ToSharedRef(), &FExternalMemory_EQS::OnEQSQueryCompleted);
	bool bStarted = EQSMemory->Execute(EQSRequest, Actor, EQSParams);
	return bStarted;
}

bool FTrainingEpisodeSetupActionExternalMemory_EQS::Execute(const FEQSParametrizedQueryExecutionRequest& InRequest, AActor* InOwner, 
	const TArray<FMLSetupActionEQSParameter>& EQSParams)
{
	Owner = InOwner;
	Request = InRequest;
	Request.InitForOwnerAndBlackboard(*Owner.Get(), nullptr);
	for (const auto& EQSParam : EQSParams)
	{
		FAIDynamicParam NewEQSParam;
		NewEQSParam.ParamName = EQSParam.Name;
		NewEQSParam.bAllowBBKey = 0;
		NewEQSParam.ParamType = EQSParam.Type;
		NewEQSParam.Value = EQSParam.Value;
		Request.QueryConfig.Add(NewEQSParam);
	}
	
	bEQSSucceeded = false;
	int32 EqsRequestId = Request.Execute(*Owner.Get(), nullptr, QueryFinishedDelegate);
	return EqsRequestId != INDEX_NONE;
}

void FTrainingEpisodeSetupActionExternalMemory_EQS::OnEQSQueryCompleted(TSharedPtr<FEnvQueryResult> Result)
{
	if (Result->IsAborted())
		return;

	bool bSuccess = Result->IsSuccessful() && (Result->Items.Num() >= 1);
	if (!bSuccess)
		return;
	
	AActor* Actor = Result->GetItemAsActor(0);
	bSuccess = false;
	if (ResultActor != nullptr)
	{
		bSuccess = true;
		OnEQSQueryActorResult(Actor);
	}
	else
	{
		FVector Location = Result->GetItemAsLocation(0);
		if (Location != FVector::ZeroVector && Location != FAISystem::InvalidLocation)
		{
			bSuccess = true;
			OnEQSQueryLocationResult(Location);
		}
	}
	
	bEQSSucceeded = bSuccess;
}
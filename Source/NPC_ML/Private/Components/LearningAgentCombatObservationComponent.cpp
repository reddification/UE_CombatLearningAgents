#include "Components/LearningAgentCombatObservationComponent.h"

#include "Data/AgentCombatDataTypes.h"
#include "Data/LearningAgentsLogChannels.h"
#include "Settings/CombatLearningSettings.h"

ULearningAgentCombatObservationComponent::ULearningAgentCombatObservationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void ULearningAgentCombatObservationComponent::BeginPlay()
{
	Super::BeginPlay();
	Settings = GetDefault<UCombatLearningSettings>();
	int DownwardRaindropsResolution = Settings->DownwardRaindropsParams.Radius * 2 / Settings->DownwardRaindropsParams.Density;
	int ForwardRaindropsResolution = Settings->ForwardRaindropsParams.Radius * 2 / Settings->ForwardRaindropsParams.Density;
	int BackwardRaindropsResolution = Settings->BackwardRaindropsParams.Radius * 2 / Settings->BackwardRaindropsParams.Density;
	
	LidarObservationCache.DownwardRaindrops.SetNum(DownwardRaindropsResolution * DownwardRaindropsResolution);
	LidarObservationCache.ForwardRaindrops.SetNum(ForwardRaindropsResolution * ForwardRaindropsResolution);
	LidarObservationCache.BackwardRaindrops.SetNum(BackwardRaindropsResolution * BackwardRaindropsResolution);
}

bool ULearningAgentCombatObservationComponent::IsAsyncRaindropActive() const
{
	return RaindropDownwardsVariables.bActive.load()
		|| RaindropForwardVariables.bActive.load()
		|| RaindropBackwardsVariables.bActive.load();
}

void ULearningAgentCombatObservationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	LidarCancellationToken.store(true);
	bool bAnyRaindropIsActive = IsAsyncRaindropActive();
	while (bAnyRaindropIsActive)
	{
		FPlatformProcess::Sleep(0.005f);
		bAnyRaindropIsActive = IsAsyncRaindropActive();
	}
		
	Super::EndPlay(EndPlayReason);
}

void ULearningAgentCombatObservationComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	auto OwnerLocal = GetOwner();
	auto AgentLocation = OwnerLocal->GetActorLocation();
	auto AgentForwardVector = OwnerLocal->GetActorForwardVector();
	auto AgentRightVector = OwnerLocal->GetActorRightVector();
	
	FHitResult CeilingHitResult;
	const FVector CeilingEndTraceLocation = AgentLocation + FVector::UpVector * Settings->DownwardRaindropsParams.Radius;
	bool bHit = GetWorld()->LineTraceSingleByChannel(CeilingHitResult, AgentLocation, CeilingEndTraceLocation, RaindropCollisionChannel);
	LidarObservationCache.AverageCeilingHeight = CeilingHitResult.Distance;
	
	const FVector RaindropDownwardOrigin(AgentLocation.X, AgentLocation.Y, CeilingHitResult.TraceEnd.Z);
	
	{
		FScopeLock Lock(&RaindropDownwardsVariables.CriticalSection);
		RaindropDownwardsVariables.OriginVector = RaindropDownwardOrigin;
		RaindropDownwardsVariables.DirectionVectorX = AgentForwardVector;
		RaindropDownwardsVariables.DirectionVectorY = AgentRightVector;
		RaindropDownwardsVariables.TraceDirection = FVector::DownVector;
	}
	
	{
		FScopeLock Lock(&RaindropForwardVariables.CriticalSection);
		RaindropForwardVariables.OriginVector = AgentLocation;
		RaindropForwardVariables.DirectionVectorX = FVector::UpVector;
		RaindropForwardVariables.DirectionVectorY = AgentRightVector;
		RaindropForwardVariables.TraceDirection = AgentForwardVector;
	}
	
	{
		FScopeLock Lock(&RaindropBackwardsVariables.CriticalSection);
		RaindropBackwardsVariables.OriginVector = AgentLocation;
		RaindropBackwardsVariables.DirectionVectorX = FVector::UpVector;
		RaindropBackwardsVariables.DirectionVectorY = AgentRightVector;
		RaindropBackwardsVariables.TraceDirection = -AgentForwardVector;
	}
	
	if (!RaindropDownwardsVariables.bActive.load())
	{
		LidarRaindropAsync(RaindropDownwardsVariables, Settings->DownwardRaindropsParams,
			LidarObservationCache.DownwardRaindrops.GetData(), &RaindropDownwardsVariables.bActive, 1);	
	}
	
	if (!RaindropForwardVariables.bActive.load())
	{
		LidarRaindropAsync(RaindropForwardVariables, Settings->ForwardRaindropsParams,
			LidarObservationCache.ForwardRaindrops.GetData(), &RaindropForwardVariables.bActive, 2);	
	}
	
	if (!RaindropBackwardsVariables.bActive.load())
	{
		LidarRaindropAsync(RaindropBackwardsVariables, Settings->BackwardRaindropsParams,
			LidarObservationCache.BackwardRaindrops.GetData(), &RaindropBackwardsVariables.bActive, 3);	
	}
}

FSelfData ULearningAgentCombatObservationComponent::GetSelfData() const
{
	FSelfData SelfData;

	auto OwnerActor = GetOwner();
	SelfData.NormalizedHealth = GetNormalizedHealth();
	SelfData.NormalizedStamina = GetNormalizedStamina();
	SelfData.ArmorRate = GetArmorRate();
	SelfData.CombatStates = GetCombatStates();
	SelfData.Level = GetLevel();
	SelfData.SurvivalDesire = GetSurvivalDesire();
	SelfData.WeaponData = GetWeaponData();
	SelfData.WorldVelocity = OwnerActor->GetVelocity();
	SelfData.ActiveGesture = GetActiveGesture(OwnerActor);
	
	return SelfData;
}

FCombatStateData ULearningAgentCombatObservationComponent::GetCombatStateData() const
{
	FCombatStateData Result;
	Result.CombatTotalDuration = GetWorld()->GetTimeSeconds() - CombatStartTime;
	return Result;
}

TArray<FEnemyData> ULearningAgentCombatObservationComponent::GetEnemies() const
{
	return {};
}

TArray<FAllyData> ULearningAgentCombatObservationComponent::GetAllies() const
{
	return {};
}

void ULearningAgentCombatObservationComponent::OnCombatStarted()
{
	CombatStartTime = GetWorld()->GetTimeSeconds();
	LidarCancellationToken.store(false);
	SetComponentTickEnabled(true);
}

void ULearningAgentCombatObservationComponent::OnCombatEnded()
{
	LidarCancellationToken.store(true);
	SetComponentTickEnabled(false);
}

// TODO use (and adjust if needed) Nav3D plugin to prepare same data as linetrace raindrops
void ULearningAgentCombatObservationComponent::CollectSpatialObservation_Octree()
{
	// const FVector Offset(SpatialAwarenessObservationRadiusXY, SpatialAwarenessObservationRadiusXY, SpatialAwarenessObservationRadiusZ);
	// auto AgentLocation = GetOwner()->GetActorLocation();
	// auto SpatialObservationsRoot = AgentLocation + (-Offset);
	// ANav3DData* NavData = FNav3DUtils::GetNav3DData(GetWorld());
	// const FNav3DVolumeNavigationData* Nav3DVolumeNavigationData = NavData->GetVolumeNavigationDataContainingPoints({ SpatialObservationsRoot  });
	// const FNav3DVolumeNavigationData* Nav3DVolumeNavigationData_Test = NavData->GetVolumeNavigationDataContainingPoints({ AgentLocation });
	// if (Nav3DVolumeNavigationData != Nav3DVolumeNavigationData_Test)
	// {
	// 	ensure(false); // idk what's going to happen	
	// }
	//
	// FNav3DNodeAddress CurrentNodeAddress;
	// bool bFound = Nav3DVolumeNavigationData->GetNodeAddressFromPosition(CurrentNodeAddress, AgentLocation, 0);
	// if (!ensure(bFound))
	// 	return;
	//
	// TArray<FNav3DNodeAddress> NeighborsNodesAddresses_Test;
	// Nav3DVolumeNavigationData->GetNodeNeighbours(NeighborsNodesAddresses_Test, CurrentNodeAddress);
	// const FNav3DNode& CurrentNode = Nav3DVolumeNavigationData->GetNodeFromAddress(CurrentNodeAddress);
	// const auto& NeighborsAddresses = CurrentNode.Neighbours;
	// ensure(NeighborsNodesAddresses_Test.Num() == 6);
	// const float AgentRadius = 25.f; // TODO get from somewhere
	// const int CountXY = FMath::CeilToInt32(SpatialAwarenessObservationRadiusXY * 2.f / AgentRadius);
	// const int CountZ = FMath::CeilToInt32(SpatialAwarenessObservationRadiusZ * 2.f / AgentRadius);
	// auto NextNodeAddress = CurrentNodeAddress;
	// 	// TODO paralellize by vertical component
	//
	// // directions are taken from GNeighbourDirections array (Nav3DTypes.h)
	// const int DirectionUp = 4;
	// const int DirectionForward = 0;
	// const int DirectionRight = 2;
	// for (int i = 0; i < CountXY; i++)
	// {
	// 	const auto& StartNode = CurrentNode;
	// 	for (int j = 0; j < CountXY; j++)
	// 	{
	// 		for (int k = 0; k < CountZ; k++)
	// 		{
	// 			// bool IsOccluded = Nav3DVolumeNavigationData->IsPositionOccluded()				
	// 		}
	// 	}
	// 	
	// 	NextNodeAddress = CurrentNode.Neighbours[4];
	// }		
	
}

// raindrop approach is probably the most accurate, but also the least scalable
// I mean even if I put all raindrops in 1 thread, it's still like no more than 8-2=6 agents on average
// TODO find a proper octree implementation and use it instead
void ULearningAgentCombatObservationComponent::LidarRaindropAsync(const FLidarRaindropVariables& RaindropVariables,
                                                                  const FLidarRaindropParams& RaindropParams, float* RawData, 
                                                                  std::atomic<bool>* bActiveFlag, int RaindropLogId)
{
	Async(EAsyncExecution::Thread, [this, &RaindropVariables, RawData, &RaindropParams, RaindropLogId, bActiveFlag]()
	{
		int RaindropResolution = RaindropParams.Radius * 2 / RaindropParams.Density;
		bActiveFlag->store(true);
		const double StartTime = FPlatformTime::Seconds();
		// x - row, y - column
		bool bCancellationRequested = LidarCancellationToken.load();
		for (int x = 0; x < RaindropResolution && !bCancellationRequested; x+=2)
		{
			Raindrop(RaindropVariables, RaindropParams, RawData, RaindropResolution, x);
			bCancellationRequested = LidarCancellationToken.load();
		}
		
		bCancellationRequested = LidarCancellationToken.load();
		for (int x = 1; x < RaindropResolution && !bCancellationRequested; x+=2)
		{
			Raindrop(RaindropVariables, RaindropParams, RawData, RaindropResolution, x);
			bCancellationRequested = LidarCancellationToken.load();
		}
		
		const double EndTime = FPlatformTime::Seconds();
		UE_LOG(LogLA_Combat_Observations_Raindrop, Log, TEXT("Raindrop %d finished. Duration: %.2f s"), RaindropLogId, EndTime - StartTime);
		
		bActiveFlag->store(false);
	});
}

void ULearningAgentCombatObservationComponent::Raindrop(const FLidarRaindropVariables& RaindropVariables, const FLidarRaindropParams& RaindropParams, 
	float* RawData, int RaindropResolution, int x) const
{
	if (LidarCancellationToken.load())
		return;
	
	FVector ActualDirectionX;
	FVector ActualDirectionY;
	FVector ActualTraceDirection;
	FVector ActualOriginVector;
	
	{
		FScopeLock Lock(&RaindropVariables.CriticalSection);
		ActualDirectionX = RaindropVariables.DirectionVectorX;
		ActualDirectionY = RaindropVariables.DirectionVectorY;
		ActualTraceDirection = RaindropVariables.TraceDirection;
		ActualOriginVector = RaindropVariables.OriginVector;
	}
	
	ensure(ActualTraceDirection.IsNormalized());
				
	const FVector StartPoint = ActualOriginVector 
		+ ActualDirectionX * (x * RaindropParams.Density - RaindropParams.Radius)  
		+ (-ActualDirectionY * RaindropParams.Radius);
	TArray<float> RaindropBatch;
	RaindropBatch.SetNum(RaindropResolution);
	for (int y = 0; y < RaindropResolution; y++)
	{
		FHitResult HitResult;
		const FVector TraceStart = StartPoint + ActualDirectionY * RaindropParams.Density * y;
		const FVector TraceEnd = TraceStart + ActualTraceDirection * RaindropParams.Radius * 2.f;
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, RaindropCollisionChannel);
		RaindropBatch[y] = HitResult.Distance / RaindropParams.Radius * 2.f;
	}
			
	if (LidarCancellationToken.load())
		return;
	
	AsyncTask(ENamedThreads::Type::GameThread, [&, RaindropBatch = MoveTemp(RaindropBatch)] ()
	{
		memcpy(RawData + x * RaindropResolution, RaindropBatch.GetData(), RaindropResolution * sizeof(float));
	});
}

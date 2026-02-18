#include "Components/LACombatObservationComponent.h"

#include "Data/AgentCombatDataTypes.h"
#include "Data/LearningAgentsLogChannels.h"
#include "Data/LogChannels.h"
#include "Settings/CombatLearningSettings.h"

ULACombatObservationComponent::ULACombatObservationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void ULACombatObservationComponent::BeginPlay()
{
	Super::BeginPlay();
	Settings = GetDefault<UCombatLearningSettings>();
	const int DownwardRaindropsResolution = Settings->DownwardRaindropsParams.GetResolution();
	const int ForwardRaindropsResolution = Settings->ForwardRaindropsParams.GetResolution();
	const int BackwardRaindropsResolution = Settings->BackwardRaindropsParams.GetResolution();
	
	SelfDownwardRaindrop.Variables.TraceDistance = Settings->DownwardRaindropsParams.TraceDistance;
	SelfForwardRaindrop.Variables.TraceDistance = Settings->ForwardRaindropsParams.TraceDistance;
	SelfBackwardRaindrop.Variables.TraceDistance = Settings->BackwardRaindropsParams.TraceDistance;

	FRaindropBuffer SelfDownwardRaindropBuffer(DownwardRaindropsResolution * DownwardRaindropsResolution, SelfDownwardRaindrop.BufferHandle.GetOccupierId());
	FRaindropBuffer SelfForwardRaindropBuffer(ForwardRaindropsResolution * ForwardRaindropsResolution, SelfForwardRaindrop.BufferHandle.GetOccupierId());
	FRaindropBuffer SelfBackwardRaindropBuffer(BackwardRaindropsResolution * BackwardRaindropsResolution, SelfBackwardRaindrop.BufferHandle.GetOccupierId());
	
	RaindropBuffers.Add(ELAAgentAttitude::Self);
	RaindropBuffers[ELAAgentAttitude::Self].Add(MoveTemp(SelfDownwardRaindropBuffer));	
	RaindropBuffers[ELAAgentAttitude::Self].Add(MoveTemp(SelfForwardRaindropBuffer));	
	RaindropBuffers[ELAAgentAttitude::Self].Add(MoveTemp(SelfBackwardRaindropBuffer));	
	
	LidarSelfObservationCache.DownwardRaindrops = &RaindropBuffers[ELAAgentAttitude::Self][0].RawData;
	LidarSelfObservationCache.ForwardRaindrops = &RaindropBuffers[ELAAgentAttitude::Self][1].RawData;
	LidarSelfObservationCache.BackwardRaindrops = &RaindropBuffers[ELAAgentAttitude::Self][2].RawData;

#if WITH_EDITOR
	SelfDownwardRaindrop.Variables.LogInfo = TEXT("self downwards");
	SelfForwardRaindrop.Variables.LogInfo = TEXT("self forward");
	SelfBackwardRaindrop.Variables.LogInfo = TEXT("self downward");
#endif
	
}

void ULACombatObservationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ResetRaindropBuffersTimer.IsValid())
		if (auto World = GetWorld())
			World->GetTimerManager().ClearTimer(ResetRaindropBuffersTimer);
	
	LidarCancellationToken.store(true);
	bool bAnyRaindropIsActive = IsAsyncRaindropActive();
	while (bAnyRaindropIsActive)
	{
		FPlatformProcess::Sleep(0.005f);
		bAnyRaindropIsActive = IsAsyncRaindropActive();
	}
		
	Super::EndPlay(EndPlayReason);
}

void ULACombatObservationComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	auto OwnerLocal = GetOwner();
	auto AgentLocation = OwnerLocal->GetActorLocation();
	auto AgentForwardVector = OwnerLocal->GetActorForwardVector();
	auto AgentRightVector = OwnerLocal->GetActorRightVector();
	
	FHitResult CeilingHitResult;
	const FVector CeilingEndTraceLocation = AgentLocation + FVector::UpVector * Settings->DownwardRaindropsParams.Radius;
	bool bHit = GetWorld()->LineTraceSingleByChannel(CeilingHitResult, AgentLocation, CeilingEndTraceLocation, Settings->RaindropCollisionChannel);
	LidarSelfObservationCache.AverageCeilingHeight = CeilingHitResult.Distance;
	
	
	{
		FScopeLock Lock(&SelfDownwardRaindrop.Variables.SynchronizationUnit->CriticalSection);
		SelfDownwardRaindrop.Variables.OriginVector = FVector(AgentLocation.X, AgentLocation.Y, CeilingHitResult.TraceEnd.Z);
		SelfDownwardRaindrop.Variables.DirectionVectorX = AgentForwardVector;
		SelfDownwardRaindrop.Variables.DirectionVectorY = AgentRightVector;
		SelfDownwardRaindrop.Variables.TraceDirection = FVector::DownVector;
	}
	
	{
		FScopeLock Lock(&SelfForwardRaindrop.Variables.SynchronizationUnit->CriticalSection);
		SelfForwardRaindrop.Variables.OriginVector = AgentLocation;
		SelfForwardRaindrop.Variables.DirectionVectorX = FVector::UpVector;
		SelfForwardRaindrop.Variables.DirectionVectorY = AgentRightVector;
		SelfForwardRaindrop.Variables.TraceDirection = AgentForwardVector;
	}
	
	{
		FScopeLock Lock(&SelfBackwardRaindrop.Variables.SynchronizationUnit->CriticalSection);
		SelfBackwardRaindrop.Variables.OriginVector = AgentLocation;
		SelfBackwardRaindrop.Variables.DirectionVectorX = FVector::UpVector;
		SelfBackwardRaindrop.Variables.DirectionVectorY = AgentRightVector;
		SelfBackwardRaindrop.Variables.TraceDirection = -AgentForwardVector;
	}
	
	if (!SelfDownwardRaindrop.Variables.SynchronizationUnit->bActive.load())
		LidarRaindropAsync(&SelfDownwardRaindrop.Variables, &Settings->DownwardRaindropsParams, &SelfDownwardRaindrop.BufferHandle, ELAAgentAttitude::Self);	
	
	if (!SelfForwardRaindrop.Variables.SynchronizationUnit->bActive.load())
		LidarRaindropAsync(&SelfForwardRaindrop.Variables, &Settings->ForwardRaindropsParams, &SelfForwardRaindrop.BufferHandle, ELAAgentAttitude::Self);	
	
	if (!SelfBackwardRaindrop.Variables.SynchronizationUnit->bActive.load())
		LidarRaindropAsync(&SelfBackwardRaindrop.Variables, &Settings->BackwardRaindropsParams, &SelfBackwardRaindrop.BufferHandle, ELAAgentAttitude::Self);	

	UpdateRaindropsToTargets(CachedEnemiesData, ELAAgentAttitude::Enemy);
	UpdateRaindropsToTargets(CachedAlliesData, ELAAgentAttitude::Ally);
}

bool ULACombatObservationComponent::IsAsyncRaindropActive() const
{
	return ActiveRaindropsCount.load() > 0;
}

void ULACombatObservationComponent::UpdateRaindropsToTargets(FCharacterDataContainer& CharactersData, ELAAgentAttitude TargetType)
{
	auto AgentLocation = GetOwner()->GetActorLocation();
	auto AgentUpVector = GetOwner()->GetActorUpVector();
	for (auto& CachedCharacter : CharactersData)
	{
		if (!ensure(CachedCharacter.Value->RaindropData.BufferHandle.IsValid()) || !CachedCharacter.Value->RaindropData.bRelevant)
			continue;
		
		{
			FScopeLock Lock(&CachedCharacter.Value->RaindropData.Variables.SynchronizationUnit->CriticalSection);
			FVector AgentToEnemy = CachedCharacter.Value->LastKnownLocation - AgentLocation;
			CachedCharacter.Value->RaindropData.Variables.TraceDistance = AgentToEnemy.Size();
			CachedCharacter.Value->RaindropData.Variables.TraceDirection = AgentToEnemy.GetSafeNormal();
			CachedCharacter.Value->RaindropData.Variables.OriginVector = AgentLocation;
			CachedCharacter.Value->RaindropData.Variables.DirectionVectorX = FVector::CrossProduct(AgentUpVector, CachedCharacter.Value->RaindropData.Variables.TraceDirection);	
			CachedCharacter.Value->RaindropData.Variables.DirectionVectorY = AgentUpVector;
		}
		
		if (!CachedCharacter.Value->RaindropData.Variables.SynchronizationUnit->bActive.load())
		{
			LidarRaindropAsync(&CachedCharacter.Value->RaindropData.Variables, &Settings->RaindropParams[TargetType], &CachedCharacter.Value->RaindropData.BufferHandle, 
			                   TargetType);
		}
	}
}

bool ULACombatObservationComponent::IsCharacterRelevantForRaindrop(const FPerceivedCharacterData& CharacterState, const FVector& LastKnownLocation,
                                                                              const FRaindropRelevancyParams& RelevancyParams) const
{
	if (!CharacterState.bAlive)
		return false;
	
	auto OwnerLocal = GetOwner();
	const FVector AgentToCharacter = LastKnownLocation - OwnerLocal->GetActorLocation();
	const float Distance = AgentToCharacter.Size();
	const FVector AgentToCharacterDirection = AgentToCharacter.GetSafeNormal();
	const float AgentToCharacterDotProduct = OwnerLocal->GetActorForwardVector() | AgentToCharacterDirection;
	const float DistanceAlpha = FMath::Min( Distance / RelevancyParams.DistanceThreshold, 1.f);
	bool bRelevant = Distance <= RelevancyParams.DistanceThreshold
		&& AgentToCharacterDotProduct >= RelevancyParams.DotProductThreshold;
	
	if (!bRelevant)
		return false;
	
	const float DistanceAttenuatedDotRelevancy = FMath::Lerp(1.f, AgentToCharacterDotProduct, DistanceAlpha);
	bRelevant = bRelevant && DistanceAttenuatedDotRelevancy >= RelevancyParams.DistanceAttenuatedDotProductThreshold;
	return bRelevant;
}

void ULACombatObservationComponent::StopRaindrop(FRaindrop& RaindropData, ELAAgentAttitude Attitude)
{
	if (!ensure(IsInGameThread()))
		return;
	
	RaindropData.Variables.SynchronizationUnit->bActive.store(false);
	RaindropBuffers[Attitude][RaindropData.BufferHandle.RaindropBufferIndex].OccupierId = {};
	RaindropBuffers[Attitude][RaindropData.BufferHandle.RaindropBufferIndex].RawData.Reset();
	RaindropData.BufferHandle.RaindropBufferIndex = -1;
}

FSelfData ULACombatObservationComponent::GetSelfData() const
{
	FSelfData SelfData;

	auto OwnerActor = GetOwner();
	SelfData.NormalizedHealth = GetNormalizedHealth(OwnerActor);
	SelfData.NormalizedStamina = GetNormalizedStamina(OwnerActor);
	SelfData.NormalizedPoise = GetNormalizedPoise(OwnerActor);
	SelfData.ArmorRate = GetArmorRate(OwnerActor);
	SelfData.CombatStates = GetCombatStates(OwnerActor);
	SelfData.Level = GetLevel(OwnerActor);
	SelfData.WeaponData = GetWeaponData(OwnerActor);
	SelfData.WorldVelocity = OwnerActor->GetVelocity();
	SelfData.ActiveGesture = GetActiveGesture(OwnerActor);
	SelfData.ActivePhrase = GetActivePhrase(OwnerActor);
	SelfData.AccumulatedNormalizedDamage = GetAccumulatedNormalizedDamage(OwnerActor);
	
	return SelfData;
}

FCombatStateData ULACombatObservationComponent::GetCombatStateData() const
{
	FCombatStateData Result;
	Result.CombatTotalDuration = GetWorld()->GetTimeSeconds() - CombatStartTime;
	return Result;
}

bool ULACombatObservationComponent::OccupyRaindropBuffer(ELAAgentAttitude RaindropTarget, FRaindropBufferHandle& Handle)
{
	for (int i = 0; i < RaindropBuffers[RaindropTarget].Num(); i++)
	{
		if (!RaindropBuffers[RaindropTarget][i].IsOccupied())
		{
			RaindropBuffers[RaindropTarget][i].OccupierId = Handle.GetOccupierId();
			Handle.RaindropBufferIndex = i;
			return true;
		}
	}
	
	return false;
}

void ULACombatObservationComponent::ProcessTargetObservations(TArray<TSharedRef<FPerceivedCharacterData>>&& Targets, ELAAgentAttitude RaindropTarget)
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	int MaxCountOfTargets = 0;
	FCharacterDataContainer* CachedTargetsData = nullptr;
	switch (RaindropTarget)
	{
		case ELAAgentAttitude::Enemy:
			CachedTargetsData = &CachedEnemiesData;
			MaxCountOfTargets = Settings->MaxEnemiesObservedAtOnce;
			break;
		case ELAAgentAttitude::Ally:
			CachedTargetsData = &CachedAlliesData;
			MaxCountOfTargets = Settings->MaxAlliesObservedAtOnce;
			break;
		default:
			ensure(false);
		break;
	}
	
	if (CachedTargetsData == nullptr)
		return;

	// sort and shrink enemies
	Targets.Sort([OwnerLocation](const TSharedPtr<FPerceivedCharacterData>& A, const TSharedPtr<FPerceivedCharacterData>& B)
	{
		if (!A->bAgentCanSeeCharacter && B->bAgentCanSeeCharacter || !A->bAlive && B->bAlive)
			return false;
		
		if (A->bAgentCanSeeCharacter && !B->bAgentCanSeeCharacter || A->bAlive && !B->bAlive)
			return true;
			
		return (A->Actor->GetActorLocation() - OwnerLocation).SizeSquared() < (B->Actor->GetActorLocation() - OwnerLocation).SizeSquared();
	});
	if (Targets.Num() > MaxCountOfTargets)
		Targets.RemoveAt(MaxCountOfTargets, Targets.Num() - MaxCountOfTargets);
	
	// remove those that don't present anymore
	TSet<AActor*> TargetActorsSet;
	TargetActorsSet.Reserve(Targets.Num());
	for (int i = 0; i < Targets.Num(); i++)
		TargetActorsSet.Add(Targets[i]->Actor.Get());
	
	TSet<AActor*> NoMorePresentCharacters;
	NoMorePresentCharacters.Reserve(CachedTargetsData->Num() - Targets.Num());
	for (auto& CachedEnemy : *CachedTargetsData)
	{
		auto CachedEnemyActor = CachedEnemy.Key.Get();
		if (!TargetActorsSet.Contains(CachedEnemyActor))
		{
			StopRaindrop(CachedEnemy.Value->RaindropData, RaindropTarget);
			NoMorePresentCharacters.Add(CachedEnemyActor);
		}
	}
	
	for (const auto& NoMorePresentCharacter : NoMorePresentCharacters)
		CachedTargetsData->Remove(NoMorePresentCharacter);
	
	const float WorldTime = GetWorld()->GetTimeSeconds();
	// add that aren't present now
	// update those that present
	
	for (int i = 0; i < Targets.Num(); i++)
	{
		if (CachedTargetsData->Contains(Targets[i]->Actor))
		{
			auto& KnownActorObservation = (*CachedTargetsData)[Targets[i]->Actor];
			const bool bOldRelevant = KnownActorObservation->RaindropData.bRelevant;
			if (Targets[i]->bAgentCanSeeCharacter)
				KnownActorObservation->LastKnownLocation = Targets[i]->Actor->GetActorLocation();
			
			KnownActorObservation->CharacterState = Targets[i];
			bool bNewRelevant = IsCharacterRelevantForRaindrop(Targets[i].Get(), KnownActorObservation->LastKnownLocation, 
				Settings->RaindropRelevanceParams[RaindropTarget]);
			KnownActorObservation->RaindropData.bRelevant = bNewRelevant;
			if (bNewRelevant)
			{
				KnownActorObservation->RaindropData.LastRelevantAt = WorldTime;
				KnownActorObservation->RaindropData.Variables.SynchronizationUnit->bActive.store(false);
			}
			else
			{
				const bool bTimeRelevancyExpired = KnownActorObservation->RaindropData.LastRelevantAt > 0.f 
					&& WorldTime - KnownActorObservation->RaindropData.LastRelevantAt > Settings->RaindropRelevanceParams[RaindropTarget].IrrelevancyTimeDelay; 
				if (bTimeRelevancyExpired && KnownActorObservation->RaindropData.Variables.SynchronizationUnit->bActive.load())
					StopRaindrop(KnownActorObservation->RaindropData, RaindropTarget);
			}
		}
		else 
		{
			TSharedPtr<FOtherCharacterObservationData> NewEnemyObservationData = MakeShared<FOtherCharacterObservationData>();
			NewEnemyObservationData->CharacterState = Targets[i];
			NewEnemyObservationData->LastKnownLocation = Targets[i]->Actor->GetActorLocation();
			NewEnemyObservationData->RaindropData.bRelevant = IsCharacterRelevantForRaindrop(Targets[i].Get(), NewEnemyObservationData->LastKnownLocation,
				Settings->RaindropRelevanceParams[RaindropTarget]);
			bool bBufferOccupied = OccupyRaindropBuffer(RaindropTarget, NewEnemyObservationData->RaindropData.BufferHandle);
			ensure(bBufferOccupied);
			CachedTargetsData->Add(Targets[i]->Actor, NewEnemyObservationData);
		}
	}
}

const TMap<TWeakObjectPtr<AActor>, TSharedPtr<FOtherCharacterObservationData>>& ULACombatObservationComponent::GetEnemiesObservationData()
{
	TArray<TSharedRef<FPerceivedCharacterData>> Targets = GetEnemies();
	ProcessTargetObservations(MoveTemp(Targets), ELAAgentAttitude::Enemy);
	return CachedEnemiesData;
}

const TMap<TWeakObjectPtr<AActor>, TSharedPtr<FOtherCharacterObservationData>>& ULACombatObservationComponent::GetAlliesObservationData()
{
	TArray<TSharedRef<FPerceivedCharacterData>> Targets = GetAllies();
	ProcessTargetObservations(MoveTemp(Targets), ELAAgentAttitude::Ally);
	return CachedAlliesData;
}

TArray<TSharedRef<FPerceivedCharacterData>> ULACombatObservationComponent::GetEnemies() const
{
	unimplemented();
	return {};
}

TArray<TSharedRef<FPerceivedCharacterData>> ULACombatObservationComponent::GetAllies() const
{
	unimplemented();
	return {};
}

void ULACombatObservationComponent::OnCombatStarted()
{
	CombatStartTime = GetWorld()->GetTimeSeconds();
	LidarCancellationToken.store(false);
	SetComponentTickEnabled(true);
	if (ResetRaindropBuffersTimer.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(ResetRaindropBuffersTimer);
	
	RaindropBuffers.Add(ELAAgentAttitude::Enemy, {});
	RaindropBuffers.Add(ELAAgentAttitude::Ally, {});
	
	const int RaindropToEnemyResolutionSq = FMath::Square(Settings->RaindropParams[ELAAgentAttitude::Enemy].GetResolution());
	for (int i = 0; i < Settings->MaxEnemiesObservedAtOnce; i++)
	{
		FRaindropBuffer Buffer;
		RaindropBuffers[ELAAgentAttitude::Enemy].Add(Buffer);
		RaindropBuffers[ELAAgentAttitude::Enemy][i].RawData.SetNumZeroed(RaindropToEnemyResolutionSq);
	}
	
	const int RaindropToAllyResolutionSq = FMath::Square(Settings->RaindropParams[ELAAgentAttitude::Ally].GetResolution());
	for (int i = 0; i < Settings->MaxAlliesObservedAtOnce; i++)
	{
		FRaindropBuffer Buffer;
		RaindropBuffers[ELAAgentAttitude::Ally].Add(Buffer);
		RaindropBuffers[ELAAgentAttitude::Ally][i].RawData.SetNumZeroed(RaindropToAllyResolutionSq);
	}
}

void ULACombatObservationComponent::ResetRaindropBuffers()
{
	RaindropBuffers.Remove(ELAAgentAttitude::Ally);
	RaindropBuffers.Remove(ELAAgentAttitude::Enemy);
}

void ULACombatObservationComponent::OnCombatEnded()
{
	LidarCancellationToken.store(true);
	for (auto& Character : CachedEnemiesData)
		StopRaindrop(Character.Value->RaindropData, ELAAgentAttitude::Enemy);
	
	for (auto& Character : CachedAlliesData)
		StopRaindrop(Character.Value->RaindropData, ELAAgentAttitude::Ally);
	
	SetComponentTickEnabled(false);
	// 2 seconds should be enough for all raindrops to stop
	GetWorld()->GetTimerManager().SetTimer(ResetRaindropBuffersTimer, this, &ULACombatObservationComponent::ResetRaindropBuffers, 2.f);
}

bool ULACombatObservationComponent::HasRelevantLidarData(AActor* Actor, ELAAgentAttitude TargetType) const
{
	switch (TargetType)
	{
		case ELAAgentAttitude::Ally:
			{
				auto AllyData = CachedAlliesData.Find(Actor);
				return AllyData && (*AllyData)->RaindropData.bRelevant && (*AllyData)->RaindropData.BufferHandle.IsValid();
			}
		break;
		case ELAAgentAttitude::Enemy:
			{
				auto EnemyData = CachedEnemiesData.Find(Actor);
				return EnemyData && (*EnemyData)->RaindropData.bRelevant && (*EnemyData)->RaindropData.BufferHandle.IsValid();
			}
		break;
		default:
			return false;
	}
}

const TArray<float>* ULACombatObservationComponent::GetLidarDataTo(AActor* ForActor, ELAAgentAttitude TargetType) const
{
	if (!HasRelevantLidarData(ForActor, TargetType) || RaindropBuffers[TargetType].IsEmpty())
		return nullptr;

	const TSharedPtr<FOtherCharacterObservationData>* CharacterObservationData = nullptr;
	if (TargetType == ELAAgentAttitude::Ally)
		 CharacterObservationData = CachedAlliesData.Find(ForActor);
	else if (TargetType == ELAAgentAttitude::Enemy)
		CharacterObservationData = CachedEnemiesData.Find(ForActor);
	
	if (CharacterObservationData == nullptr)
		return nullptr;
	
	bool bValid = (*CharacterObservationData)->RaindropData.bRelevant && (*CharacterObservationData)->RaindropData.BufferHandle.IsValid()
		&& ensure(RaindropBuffers[TargetType].Num() > (*CharacterObservationData)->RaindropData.BufferHandle.RaindropBufferIndex);
	
	if (!bValid)
		return nullptr;
		
	return &RaindropBuffers[TargetType][(*CharacterObservationData)->RaindropData.BufferHandle.RaindropBufferIndex].RawData;
	// return &RaindropsToCharacters[ForActor].Raindrops; 
}

// TODO use (and adjust if needed) Nav3D plugin to prepare same data as linetrace raindrops
void ULACombatObservationComponent::CollectSpatialObservation_Octree()
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
// I can only excuse myself by saying that 
// 1. it's async and won't stagger the game thread
// 2. it's ok if it takes 100-150ms to complete for an agent because real humans also don't process the information immediately
// that said, still TODO find a proper octree implementation and use it instead
void ULACombatObservationComponent::LidarRaindropAsync(const FLidarRaindropVariables* RaindropVariables,
                                                                  const FLidarRaindropParams* RaindropParams, const FRaindropBufferHandle* RaindropBufferHandle, ELAAgentAttitude TargetType)
{
#if WITH_EDITOR
	UE_LOG(LogLA_Combat_Observations_Raindrop, Log, TEXT("Starting raindrop [%s]"), *RaindropVariables->LogInfo);
#endif		
	
	Async(EAsyncExecution::ThreadPool, [this, RaindropVariables, RaindropParams, RaindropBufferHandle, TargetType]()
	{
		auto& bActiveFlag = RaindropVariables->SynchronizationUnit->bActive;
		ActiveRaindropsCount.fetch_add(1, std::memory_order::relaxed);
		int RaindropResolution = RaindropParams->GetResolution();
		bActiveFlag.store(true);
		const double StartTime = FPlatformTime::Seconds();
		// x - row, y - column
		bool bCancellationRequested = LidarCancellationToken.load() || !bActiveFlag.load();
		FRaindropRowData RowData(RaindropResolution, 0, RaindropBufferHandle->RaindropBufferIndex, TargetType, RaindropBufferHandle->GetOccupierId());
		TArray<float> Array; 
		Array.SetNum(RaindropResolution * RaindropResolution);
		for (int x = 0; x < RaindropResolution && !bCancellationRequested; x+=2)
		{
			RowData.Row = x;
			RaindropToArray(RaindropVariables, RaindropParams, RowData, Array);
			// RaindropRow(RaindropVariables, RaindropParams, RowData);
			bCancellationRequested = LidarCancellationToken.load() || !bActiveFlag.load();
		}
		
		bCancellationRequested = LidarCancellationToken.load() || !bActiveFlag.load();
		for (int x = 1; x < RaindropResolution && !bCancellationRequested; x+=2)
		{
			RowData.Row = x;
			RaindropToArray(RaindropVariables, RaindropParams, RowData, Array);
			// RaindropRow(RaindropVariables, RaindropParams, RowData);
			bCancellationRequested = LidarCancellationToken.load() || !bActiveFlag.load();
		}
		
#if WITH_EDITOR
		const double EndTime = FPlatformTime::Seconds();
		UE_LOG(LogLA_Combat_Observations_Raindrop, Log, TEXT("Raindrop %s finished. Duration: %.2f s"), *RaindropVariables->LogInfo, EndTime - StartTime);
#endif		
		
		AsyncTask(ENamedThreads::Type::GameThread, [=, this, RaindropBatch = MoveTemp(Array)] ()
		{
			// it probably means callback was called when this raindrop was cancelled for
			if (!RaindropBuffers.Contains(RowData.TargetType) || RaindropBuffers[RowData.TargetType].Num() <= RowData.RaindropBufferIndex)
			{
				UE_VLOG(this, LogNpcMl, Warning, TEXT("Attempt to store raindrop data when there's no buffer with address [%d][%d]"),
					RowData.TargetType, RowData.RaindropBufferIndex);
				return;
			}
			
			auto& Buffer = RaindropBuffers[RowData.TargetType][RowData.RaindropBufferIndex];
			if (Buffer.OccupierId != RowData.OccupierId)
			{
				UE_VLOG(this, LogNpcMl, Warning, TEXT("Attempt to store raindrop data when there's an occupier mismatch: %s tried to put data for %s"),
					*RowData.OccupierId.ToString(), *Buffer.OccupierId.ToString());
				return;
			}
			
			Buffer.RawData = RaindropBatch;
			// float* RawData = Buffer.RawData.GetData();
			// memcpy(RawData, RaindropBatch.GetData(), RowData.RaindropResolution * sizeof(float));
		});
		
		ActiveRaindropsCount.fetch_add(-1, std::memory_order::relaxed);
		bActiveFlag.store(false);
		
	});
}

void ULACombatObservationComponent::RaindropRow(const FLidarRaindropVariables* RaindropVariables, const FLidarRaindropParams* RaindropParams,
                                                        const FRaindropRowData& RowData)
{
	if (LidarCancellationToken.load())
		return;
	
	FVector ActualDirectionX = FVector::ZeroVector;
	FVector ActualDirectionY = FVector::ZeroVector;
	FVector ActualTraceDirection = FVector::ZeroVector;
	FVector ActualOriginVector = FVector::ZeroVector;
	
	{
		FScopeLock Lock(&RaindropVariables->SynchronizationUnit->CriticalSection);
		ActualDirectionX = RaindropVariables->DirectionVectorX;
		ActualDirectionY = RaindropVariables->DirectionVectorY;
		ActualTraceDirection = RaindropVariables->TraceDirection;
		ActualOriginVector = RaindropVariables->OriginVector;
	}
	
	ensure(ActualTraceDirection.IsNormalized());
				
	const FVector StartPoint = ActualOriginVector 
		+ ActualDirectionX * (RowData.Row * RaindropParams->Density - RaindropParams->Radius)  
		+ (-ActualDirectionY * RaindropParams->Radius);
	TArray<float> RaindropBatch;
	RaindropBatch.SetNum(RowData.RaindropResolution);
	for (int y = 0; y < RowData.RaindropResolution; y++)
	{
		FHitResult HitResult;
		const FVector TraceStart = StartPoint + ActualDirectionY * RaindropParams->Density * y;
		const FVector TraceEnd = TraceStart + ActualTraceDirection * RaindropParams->TraceDistance;
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, Settings->RaindropCollisionChannel);
		RaindropBatch[y] = HitResult.Distance / RaindropParams->TraceDistance;
	}
			
	if (LidarCancellationToken.load() || !RaindropVariables->SynchronizationUnit->bActive.load())
		return;
	
	AsyncTask(ENamedThreads::Type::GameThread, [=, this, RaindropBatch = MoveTemp(RaindropBatch)] ()
	{
		// it probably means callback was called when this raindrop was cancelled for
		if (!RaindropBuffers.Contains(RowData.TargetType) || RaindropBuffers[RowData.TargetType].Num() <= RowData.RaindropBufferIndex)
			return;
		
		auto& Buffer = RaindropBuffers[RowData.TargetType][RowData.RaindropBufferIndex];
		if (Buffer.OccupierId != RowData.OccupierId)
		{
			UE_VLOG(this, LogNpcMl, Warning, TEXT("Attempt to store raindrop data when there's an occupier mismatch: %s tried to put data for %s"),
				*RowData.OccupierId.ToString(), *Buffer.OccupierId.ToString());
			return;
		}
		
		float* RawData = Buffer.RawData.GetData();
		memcpy(RawData + RowData.Row * RowData.RaindropResolution, RaindropBatch.GetData(), RowData.RaindropResolution * sizeof(float));
	});
}

void ULACombatObservationComponent::RaindropToArray(const FLidarRaindropVariables* RaindropVariables, const FLidarRaindropParams* RaindropParams,
                                                        const FRaindropRowData& RowData, TArray<float>& Array) const
{
	if (LidarCancellationToken.load())
		return;
	
	FVector ActualDirectionX = FVector::ZeroVector;
	FVector ActualDirectionY = FVector::ZeroVector;
	FVector ActualTraceDirection = FVector::ZeroVector;
	FVector ActualOriginVector = FVector::ZeroVector;
	
	{
		FScopeLock Lock(&RaindropVariables->SynchronizationUnit->CriticalSection);
		ActualDirectionX = RaindropVariables->DirectionVectorX;
		ActualDirectionY = RaindropVariables->DirectionVectorY;
		ActualTraceDirection = RaindropVariables->TraceDirection;
		ActualOriginVector = RaindropVariables->OriginVector;
	}
	
	ensure(ActualTraceDirection.IsNormalized());
				
	const FVector StartPoint = ActualOriginVector 
		+ ActualDirectionX * (RowData.Row * RaindropParams->Density - RaindropParams->Radius)  
		+ (-ActualDirectionY * RaindropParams->Radius);
	for (int y = 0; y < RowData.RaindropResolution; y++)
	{
		FHitResult HitResult;
		const FVector TraceStart = StartPoint + ActualDirectionY * RaindropParams->Density * y;
		const FVector TraceEnd = TraceStart + ActualTraceDirection * RaindropParams->TraceDistance;
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, Settings->RaindropCollisionChannel);
		Array[RowData.Row * RowData.RaindropResolution + y] = HitResult.Distance / RaindropParams->TraceDistance;
	}
}

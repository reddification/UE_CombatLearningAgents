#include "Components/SpatialObservationComponent_Stud.h"

#include "Data/LogChannels.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/CombatLearningSettings.h"
#include "Settings/RaindropSettings.h"

void USpatialObservationComponent_Stud::BeginPlay()
{
	Super::BeginPlay();
	AsyncRaindropsActiveCount = MakeShared<std::atomic<uint16>, ESPMode::ThreadSafe>(0);
	for (int i = 0; i < Settings->Configs.Num(); i++)
	{
		if (Settings->Configs[i].IsValid())
		{
			RaindropSchedules.Add(i, FRaindropCategorySchedule(Settings->Configs[i].Grids.Num(),
				Settings->Configs[i].UpdateInterval, this, i));
		}
	}
}

void USpatialObservationComponent_Stud::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CancelAsyncRaindrops();
	auto World = GetWorld();
	if (IsValid(World))
		World->GetTimerManager().ClearAllTimersForObject(this);
	
#if WITH_EDITOR
	int ActiveRaindropsCount = AsyncRaindropsActiveCount->load(std::memory_order::memory_order_relaxed);
	UE_LOG(LogNpcMl_Raindrop_Debug, Log, TEXT("USpatialObservationComponent_Stud::EndPlay: %d raindrops active"), ActiveRaindropsCount);
#endif
	
	bool bAnyRaindropIsActive = IsAsyncRaindropActive();
	while (bAnyRaindropIsActive)
	{
		UE_LOG(LogNpcMl_Raindrop, Warning, TEXT("USpatialObservationComponent_Stud::EndPlay: spinning sleep and waiting for raindrop to stop"))
		FPlatformProcess::Sleep(0.001f);
		bAnyRaindropIsActive = IsAsyncRaindropActive();
#if WITH_EDITOR
		ActiveRaindropsCount = AsyncRaindropsActiveCount->load(std::memory_order::memory_order_relaxed);
		UE_LOG(LogNpcMl_Raindrop_Debug, Log, TEXT("USpatialObservationComponent_Stud::EndPlay::SleepSpin: %d raindrops active"), ActiveRaindropsCount);
#endif
	}
	
	Super::EndPlay(EndPlayReason);
}

void USpatialObservationComponent_Stud::SetSpatialObservationActive_Internal(bool bActive)
{
	if (bActive && RaindropSchedules.IsEmpty())
	{
		UE_VLOG(GetOwner(), LogNpcMl_Raindrop, Warning, TEXT("Not starting raindrop because there are no schedules"));
		return;
	}
	
	Super::SetSpatialObservationActive_Internal(bActive);
	if (bActive)
	{
		for (auto& RaindropSchedule : RaindropSchedules)	
			StartAsyncRaindrop(RaindropSchedule.Value, RaindropSchedule.Key);
	}
	else
	{
		CancelAsyncRaindrops();
	}
}

void USpatialObservationComponent_Stud::ResetRaindropData()
{
	Super::ResetRaindropData();
	auto WorldLocal = GetWorld();
	for (auto& RaindropSchedule : RaindropSchedules)
		RaindropSchedule.Value.Reset(WorldLocal);
}

void USpatialObservationComponent_Stud::Debug_DoOnce()
{
	if (IsAsyncRaindropActive())
		return;
	
	for (int i = 0; i < Settings->Configs.Num(); i++)
	{
		if (Settings->Configs[i].IsValid())
		{
			if (!RaindropData.Contains(i))
			{
				FRaindropData NewPreset = FRaindropData(Settings->Configs[i].Grids.Num(), Settings->Configs[i].Params.GetResolution());
				RaindropData.Add(i, MoveTemp(NewPreset));
			}
		}
	}
	
	for (auto& RaindropSchedule : RaindropSchedules)	
		StartAsyncRaindrop(RaindropSchedule.Value, RaindropSchedule.Key);
}

#if WITH_EDITOR

void USpatialObservationComponent_Stud::Debug_CancelAsyncRaindrops()
{
	CancelAsyncRaindrops();
}

#endif

bool USpatialObservationComponent_Stud::IsAsyncRaindropActive() const
{
	return AsyncRaindropsActiveCount->load(std::memory_order::memory_order_relaxed) > 0;
}

void USpatialObservationComponent_Stud::LidarRaindropAsync_Atomic(FRaindropVariables RaindropVariables, FRaindropParams RaindropParams,
                                                                  int ConfigIndex, int GridIndex, const FRunHandle& RunHandle)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(USpatialObservationComponent_Stud::LidarRaindropAsync_Atomic);
	
	TWeakObjectPtr WorldWeak = GetWorld();
	TWeakObjectPtr ThisWeak = this;
	auto RequestedAt = FPlatformTime::Seconds();
	const uint64 CapturedRunId = RunHandle->load(std::memory_order_relaxed);
	auto Future = Async(EAsyncExecution::ThreadPool, 
		[ThisWeak, RaindropVariables, RaindropParams, WorldWeak, GridIndex, ConfigIndex, 
			RunIdRef = RunHandle.ToSharedRef(), CapturedRunId, AsyncRaindropsActiveCount = AsyncRaindropsActiveCount.ToSharedRef(), 
			CollisionQueryParams = CollisionQueryParams, RequestedAt,
			DebugOptions = DebugOptions] () mutable
	{
// 		auto IsCancelled = [=]()
// 		{
// 			TRACE_CPUPROFILER_EVENT_SCOPE(USpatialObservationComponent_Stud::LidarRaindropAsync_Atomic::CancellationCheck);
// 			// return CapturedRunId != RunId->load(std::memory_order_relaxed) || !ThisWeak.IsValid() || !WorldWeak.IsValid();
// #if WITH_EDITOR
// 			uint32 ThreadId = FPlatformTLS::GetCurrentThreadId();
// 			uint64 CurrentRunId = RunIdRef->load(std::memory_order_relaxed);
// 			UE_LOG(LogNpcMl_Raindrop_Debug, Log, TEXT("Checking is cancelled [Thread %u] Captured Run id = %llu, Current run id = %llu"), ThreadId, CapturedRunId, CurrentRunId)
// 			return CurrentRunId != CapturedRunId;
// #else
// 			return CapturedRunId != RunIdRef->load(std::memory_order_relaxed);
// #endif
// 		};
// 			
		auto CanTrace2 = [=]()
		{
			TRACE_CPUPROFILER_EVENT_SCOPE(USpatialObservationComponent_Stud::LidarRaindropAsync_Atomic::CanTrace2);
			return WorldWeak.IsValid() && !WorldWeak->bIsTearingDown;
		};
		
#if WITH_EDITOR
			auto HasDebugOption = [=](FName Name){ return DebugOptions.Contains(Name) && DebugOptions[Name]; };
			uint32 ThreadId = FPlatformTLS::GetCurrentThreadId();
			UE_LOG(LogNpcMl_Raindrop_Debug, Log, TEXT("Starting async raindrop for [%d, %d]. Thread ID: %u"), ConfigIndex, GridIndex, ThreadId);
#endif			
		// TFunctionRef<bool()> CanTrace = IsCancelled;
		TFunctionRef<bool()> CanTraceFunc = CanTrace2;
		
		// bool bCancellationRequested = IsCancelled();
		// if (IsCancelled())
		// 	return;
			
		bool bCancellationRequested = CapturedRunId != RunIdRef->load(std::memory_order_relaxed);
		if (bCancellationRequested)
			return;
			
		AsyncRaindropsActiveCount->fetch_add(1);
			
#if WITH_EDITOR
		FRaindropGridDebugData DebugData(RaindropParams, RequestedAt, RaindropVariables.CellSpan);
#endif
			
		TArray<float> Array;
		Array.SetNumUninitialized(RaindropParams.GetResolution());
			
		const int RaindropCellsPerRow = RaindropParams.Rows;

		for (int Row = 0; Row < RaindropCellsPerRow && !bCancellationRequested; Row += RaindropVariables.CellSpan)
		{
#if WITH_EDITOR
			RaindropToArray(WorldWeak, RaindropVariables, RaindropParams, CollisionQueryParams,  Array, Row, CanTraceFunc, 
				DebugData, DebugOptions);
			if (!bCancellationRequested)
			{
				Debug_StressTest(DebugOptions, FName("TryBreak_OpenLevel_AtIteration"), WorldWeak, ThisWeak);
				Debug_StressTest(DebugOptions, FName("TryBreak_DestroySelf_AtIteration"), WorldWeak, ThisWeak);
				Debug_StressTest(DebugOptions, FName("TryBreak_DestroySelf_AtIteration_Delayed"), WorldWeak, ThisWeak);
				Debug_StressTest(DebugOptions, FName("TryBreak_SpawnActorInfront"), WorldWeak, ThisWeak);
			}
			
			bool* WantStallThreadPtr = DebugOptions.Find(FName("TryBreak_StallThread_Iterations"));
			if (WantStallThreadPtr && *WantStallThreadPtr)
			{
				FPlatformProcess::Sleep(0.01f);
				UE_LOG(LogNpcMl_Raindrop_Debug, Log, TEXT("Woke up after sleeping at iteration yay [%d, %d]. Thread %u"), ConfigIndex, GridIndex, ThreadId);
			}
#else
			RaindropToArray(WorldWeak, RaindropVariables, RaindropParams, CollisionQueryParams,  Array, Row, CanTraceFunc);
#endif
			// bCancellationRequested = IsCancelled(); 
			{
				TRACE_CPUPROFILER_EVENT_SCOPE(LidarRaindropAsync::CheckCancelled)
				bCancellationRequested = CapturedRunId != RunIdRef->load(std::memory_order_relaxed);
			}
		}

#if WITH_EDITOR
		DebugData.FinishedAt = FPlatformTime::Seconds();
		UE_LOG(LogNpcMl_Raindrop_Debug, Log, TEXT("Finished raindrop loop for [%d, %d] (Thread %u). Cancellation status: %s"), ConfigIndex, GridIndex, ThreadId, bCancellationRequested ? TEXT("Cancelled") : TEXT("Not cancelled"));
#endif

		AsyncRaindropsActiveCount->fetch_add(-1);
			
		if (!bCancellationRequested)
		{
#if WITH_EDITOR
			if (HasDebugOption(FName("StressTest_BeforePublishRequested")))
			{
				Debug_StressTest(DebugOptions, FName("TryBreak_OpenLevel_AtPublish"), WorldWeak, ThisWeak);
				Debug_StressTest(DebugOptions, FName("TryBreak_DestroySelf_AtPublish"), WorldWeak, ThisWeak);
				Debug_StressTest(DebugOptions, FName("TryBreak_DestroySelf_AtPublish_Delayed"), WorldWeak, ThisWeak);
				bool* WantStallThreadPtr = DebugOptions.Find(FName("TryBreak_StallThread_AtPublish"));
				if (WantStallThreadPtr && *WantStallThreadPtr)
					FPlatformProcess::Sleep(0.5f);
				
				Debug_StressTest(DebugOptions, FName("TryBreak_CancelAtPublish"), WorldWeak, ThisWeak);
			}
#endif
			
			UE_LOG(LogNpcMl_Raindrop, Log, TEXT("Requesting publishing lambda for [%d, %d] (Thread %u)"), ConfigIndex, GridIndex, ThreadId);
			AsyncTask(ENamedThreads::Type::GameThread,
#if WITH_EDITOR
	[ThisWeak, ConfigIndex, GridIndex, RaindropBatch = MoveTemp(Array), DebugData = MoveTemp(DebugData), CapturedRunId, RunIdRef] ()
				{
					UE_LOG(LogNpcMl_Raindrop, Log, TEXT("Entered publishing lambda for [%d, %d]"), ConfigIndex, GridIndex);
					if (ThisWeak.IsValid() && CapturedRunId == RunIdRef->load(std::memory_order::relaxed))
					{
						ThisWeak->ProcessAsyncResult(RaindropBatch, ConfigIndex, GridIndex, DebugData);
					}
					else
					{
						UE_LOG(LogNpcMl_Raindrop, Log, TEXT("Can't publishing for [%d, %d] (already in game thread)"), ConfigIndex, GridIndex);
					}
				});
			
				if (HasDebugOption(FName("StressTest_AfterPublishRequested")))
				{
					Debug_StressTest(DebugOptions, FName("TryBreak_OpenLevel_AtPublish"), WorldWeak, ThisWeak);
					Debug_StressTest(DebugOptions, FName("TryBreak_DestroySelf_AtPublish"), WorldWeak, ThisWeak);
					Debug_StressTest(DebugOptions, FName("TryBreak_DestroySelf_AtPublish_Delayed"), WorldWeak, ThisWeak);
					bool* WantStallThreadPtr = DebugOptions.Find(FName("TryBreak_StallThread_AtPublish"));
					if (WantStallThreadPtr && *WantStallThreadPtr)
						FPlatformProcess::Sleep(0.5f);
								
					Debug_StressTest(DebugOptions, FName("TryBreak_CancelAtPublish"), WorldWeak, ThisWeak);
				}
			
#else
				[ThisWeak, ConfigIndex, GridIndex, RaindropBatch = MoveTemp(Array), CapturedRunId, RunIdRef] ()
				{
					if (ThisWeak.IsValid() && CapturedRunId == RunIdRef->load(std::memory_order::relaxed))
					{
						ThisWeak->ProcessAsyncResult(RaindropBatch, ConfigIndex, GridIndex);
					}
					else
					{
						UE_LOG(LogNpcMl_Raindrop, Log, TEXT("Can't publishing for [%d, %d] (already in game thread)"), ConfigIndex, GridIndex);
					}
				});
#endif
		}
		else if (ThisWeak.IsValid())
		{
#if WITH_EDITOR
			UE_LOG(LogNpcMl_Raindrop, Log, TEXT("Completed async raindrop for [%d, %d] but not publishing data because task was cancelled. Thread %u"), ConfigIndex, GridIndex, ThreadId);
#endif
		}
	});
}

void USpatialObservationComponent_Stud::LidarRaindropAsync_Stripes(FRaindropVariables RaindropVariables, FRaindropParams RaindropParams,
	int ConfigIndex, int GridIndex, const FRunHandle& RunHandle)
{
	// 7 Mar 2026 (aki):
	// honestly not sure how to do it, considering cellspan is a thing
	unimplemented();
}

// 7 Mar 2026 (aki):
// one thing to consider here that extending over grid size when cell span > 1 is not handled
// like imagine actual grid is 9 x 9, but cell span is 4. 
// it means that for last (third) iteration a shape sweep would be made that is more than half out of the real grid bounds (3 cells would be out to be precise)
// AND it would only happen for right and bottom sides of the grid. Like, right and bottom sides are LESS PRECISE
// ATM I don't really know what to do about it and should I even care, because currently I expect grids to be like ~32x32 with cell size of 25 uu and cell spans 1 to 4, 5 at max 
// so yeah well it might be overboard for right and bottom size by 1 extra cell, but is it an issue worth spending time on? idk... 
void USpatialObservationComponent_Stud::RaindropToArray(TWeakObjectPtr<UWorld> WorldWeak, const FRaindropVariables& Variables,
	const FRaindropParams& Params, const FCollisionQueryParams& CollisionQueryParams, TArray<float>& Array, int RowIndex,
	const TFunctionRef<bool()>& CanTraceFunc, 
#if WITH_EDITOR
	FRaindropGridDebugData& DebugData, TMap<FName, bool>& DebugOptions
#endif
	)
{
	// contract: starting from top left corner and going to bottom right corner
	const FVector StartPoint = Variables.OriginLocation 
		+ (-Variables.RightVector * Variables.BaseOffsetH)  
		+ (Variables.UpVector * (Variables.BaseOffsetV - RowIndex * Params.CellDimension));
	
	for (int ColumnIndex = 0; ColumnIndex < Params.Columns; ColumnIndex += Variables.CellSpan)
	{
		// if (!CanTraceFunc())
		// 	return;
	
		{
			TRACE_CPUPROFILER_EVENT_SCOPE(LidarRaindropAsync::CheckCanTrace)
			if (!WorldWeak.IsValid() || WorldWeak->bIsTearingDown)
				return;
		}
		
		FHitResult HitResult;
		// const FVector TraceStart = StartPoint + Variables.RightVector * CellCenterOffset * ColumnIndex;
		const FVector TraceStart = StartPoint + Variables.RightVector * ColumnIndex * Params.CellDimension;
		const FVector TraceEnd = TraceStart + Variables.TraceDirection * Variables.TraceDistance;
		bool bHit = false;
		switch (Params.TraceMode)
		{
			case ERaindropTraceMode::Linetrace:
				bHit = WorldWeak->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, Params.TraceChannel, CollisionQueryParams);
				break;
			case ERaindropTraceMode::BoxSweep:
			case ERaindropTraceMode::SphereSweep:
				bHit = WorldWeak->SweepSingleByChannel(HitResult, TraceStart, TraceEnd, Variables.SweepRotation,
					Params.TraceChannel, Variables.SweepShape, CollisionQueryParams);
				break;
			default:
				ensure(false);
				break;
		}
		
#if WITH_EDITOR
		{
			TRACE_CPUPROFILER_EVENT_SCOPE(LidarRaindropAsync::RaindropToArray::TraceCollection)
		FRaindropDebugData_Trace NewDebugTrace = { TraceStart, bHit ? HitResult.Location : TraceEnd, Variables.SweepRotation, Variables.SweepShape.GetSphereRadius() };
		DebugData.Traces.Add( NewDebugTrace );
		bool* StressTestKey1Ptr = DebugOptions.Find(FName("DestroyTracedObject_Instant"));
		bool* StressTestKey2Ptr = DebugOptions.Find(FName("DestroyTracedObject_Delayed"));
		bool StressTestKey1 = StressTestKey1Ptr != nullptr ? *StressTestKey1Ptr : false;
		bool StressTestKey2 = StressTestKey2Ptr != nullptr ? *StressTestKey2Ptr : false;
		if (StressTestKey1 || StressTestKey2)
		{
			if (auto Actor = HitResult.GetActor())
			{
				if (Actor->ActorHasTag(FName("RaindropStressTest")))
				{
					TWeakObjectPtr ActorWeak = Actor;
					if (StressTestKey1)
						AsyncTask(ENamedThreads::Type::GameThread, [ActorWeak](){ if (ActorWeak.IsValid()) ActorWeak->Destroy(); });
					else 
						AsyncTask(ENamedThreads::Type::GameThread, [ActorWeak](){ if (ActorWeak.IsValid()) ActorWeak->SetLifeSpan(0.0005f); });
				}
			}
		}
		}
#endif
		
		// kind of quantization but not yet. removing extra fp-digits
		float RawValue = bHit ? HitResult.Distance / Variables.TraceDistance : 1.f;
		const float UnifiedValue = bHit ? FMath::RoundToFloat(RawValue * 100000.f) / 100000.f : RawValue;
		if (Variables.CellSpan == 1)
		{
			Array[RowIndex * Params.Columns + ColumnIndex] = UnifiedValue;
		}
		else
		{
			for (int i = 0; i < Variables.CellSpan && RowIndex + i < Params.Rows; i++)
				for (int j = 0; j < Variables.CellSpan && ColumnIndex + j < Params.Columns; j++)
					Array[(RowIndex + i) * Params.Columns + ColumnIndex + j] = UnifiedValue;
		}
	}
}

void USpatialObservationComponent_Stud::RestartAsyncRaindrop(int ConfigIndex)
{
	if (bSpatialObservationActive)
		StartAsyncRaindrop(RaindropSchedules[ConfigIndex], ConfigIndex);
}

void USpatialObservationComponent_Stud::StartAsyncRaindrop(FRaindropCategorySchedule& RaindropSchedule, int ConfigIndex)
{
	RaindropSchedule.Reset(GetWorld());
	RaindropSchedule.IncrementRunHandle();
	RaindropSchedule.StartTime = FPlatformTime::Seconds();
	const auto& Grids = Settings->Configs[ConfigIndex].Grids;
	const auto& Params = Settings->Configs[ConfigIndex].Params;
	switch (AsyncProcessMode)
	{
		case ERaindropAsyncProcessMode::Atomic:
			{
				for (int i = 0; i < Grids.Num(); i++)
				{
					FRaindropVariables Variables = GetVariables(ConfigIndex, i);
					LidarRaindropAsync_Atomic(Variables, Params, ConfigIndex, i, RaindropSchedule.RunHandle);
				}
			}
			break;
		case ERaindropAsyncProcessMode::Stripes:
			{
				for (int i = 0; i < Grids.Num(); i++)
				{
					FRaindropVariables Variables = GetVariables(ConfigIndex, i);
					LidarRaindropAsync_Stripes(Variables, Params, ConfigIndex, i, RaindropSchedule.RunHandle);
				}
			}
			break;
		default:
			ensure(false);
			break;
	}
}

void USpatialObservationComponent_Stud::CancelAsyncRaindrops()
{
	// CancellationToken.Get().store(true);
	for (auto& RaindropSchedule : RaindropSchedules)
		RaindropSchedule.Value.ClearRunHandle();
}

void USpatialObservationComponent_Stud::ProcessAsyncResult(const TArray<float>& RaindropBatch, int32 ConfigIndex, int32 GridIndex,
#if WITH_EDITOR
	const FRaindropGridDebugData& DebugData
#endif
)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(USpatialObservationComponent_Stud::ProcessAsyncResult);
	
	if (!IsValid(this))
		return;
					
#if WITH_EDITOR
	TArrayView<const float> DebugArrayView = MakeArrayView(RaindropBatch);
	ProcessRaindropDebug(DebugData, DebugArrayView, ConfigIndex, GridIndex);
#endif
					
	// ThisWeak->RaindropData[GridIndex].Data = MoveTemp(RaindropBatch);
	RaindropData[ConfigIndex].StoreData(GridIndex, RaindropBatch);
	RaindropSchedules[ConfigIndex].IncrementGridsCompleted();
	if (RaindropSchedules[ConfigIndex].IsCompleted())
	{
		
#if WITH_EDITOR
		auto TotalTimeMs = (FPlatformTime::Seconds() - RaindropSchedules[ConfigIndex].StartTime) * 1000.0;
		UE_LOG(LogNpcMl_Raindrop, Log, TEXT("Raindrop for config %d took total %.2f ms"), ConfigIndex, TotalTimeMs);
#endif
		const float TimeToNextUpdate =  Settings->Configs[ConfigIndex].UpdateInterval * FMath::RandRange(0.8f, 1.2f); //randomzing to prevent reduce thread pool pressure
		GetWorld()->GetTimerManager().SetTimer(RaindropSchedules[ConfigIndex].CooldownTimer, RaindropSchedules[ConfigIndex].TimerDelegate,
			TimeToNextUpdate, false);
	}
}

#if WITH_EDITOR

void USpatialObservationComponent_Stud::Debug_StressTest(TMap<FName, bool>& DebugOptions, FName StressTestKey, TWeakObjectPtr<UWorld> WorldWeak,
	TWeakObjectPtr<USpatialObservationComponent_Stud> ThisWeak)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(LidarRaindropAsync::Debug_StressTest);
	
	bool* DebugOption = DebugOptions.Find(StressTestKey);
	if (DebugOption == nullptr || *DebugOption == false)
		return;
	
	TFunction<void()> FuncCall;
	if (StressTestKey == FName("TryBreak_OpenLevel_AtIteration") || StressTestKey == FName("TryBreak_OpenLevel_AtPublish"))
		FuncCall = [WorldWeak]() { if (WorldWeak.IsValid()) UGameplayStatics::OpenLevel(WorldWeak.Get(), FName("L_ML")); };
	else if (StressTestKey == FName("TryBreak_DestroySelf_AtIteration") || StressTestKey == FName("TryBreak_DestroySelf_AtPublish"))
		FuncCall = [ThisWeak]() { if (ThisWeak.IsValid()) ThisWeak->GetOwner()->Destroy(); };
	else if (StressTestKey == FName("TryBreak_DestroySelf_AtIteration_Delayed") || StressTestKey == FName("TryBreak_DestroySelf_AtPublish_Delayed"))
		FuncCall = [ThisWeak]() { if (ThisWeak.IsValid()) ThisWeak->GetOwner()->SetLifeSpan(0.0005f); };
	else if (StressTestKey == FName("TryBreak_SpawnActorInfront"))
		FuncCall = [ThisWeak](){ if (ThisWeak.IsValid()) ThisWeak->Debug_SpawnActorInfront(); };
	else if (StressTestKey == FName("TryBreak_CancelAtPublish"))
		FuncCall = [ThisWeak]() { if (ThisWeak.IsValid()) ThisWeak->Debug_CancelAsyncRaindrops(); };
	
	AsyncTask(ENamedThreads::Type::GameThread, FuncCall);
	DebugOptions.Remove(StressTestKey);
}

#endif
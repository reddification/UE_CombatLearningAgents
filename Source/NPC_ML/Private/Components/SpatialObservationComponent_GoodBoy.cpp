#include "Components/SpatialObservationComponent_GoodBoy.h"

#include "Data/LogChannels.h"
#include "Settings/CombatLearningSettings.h"
#include "Settings/RaindropSettings.h"

static uint32 ConfigIndexMask = 0xF0000000; // limits configs count to 16, but i doubt more that 2 will ever be needed
static uint32 ConfigIndexMaskShift = 28;
static uint32 GridIndexMask = 0x0F000000; // limits grids count to 16, but i doubt more that 10 will ever be needed
static uint32 GridIndexMaskShift = 24;
static uint32 CellSpanMask = 0x00FF0000; // limits max cell span to 256, but i doubt more that 4 will ever be needed
static uint32 CellSpanMaskShift = 16;
static uint32 ElementIndexMask = 0x0000FFFF;

void USpatialObservationComponent_GoodBoy::BeginPlay()
{
	Super::BeginPlay();
	TraceDelegate.BindUObject(this, &USpatialObservationComponent_GoodBoy::AsyncTraceCallback);
}

void USpatialObservationComponent_GoodBoy::SetSpatialObservationActive_Internal(bool bActive)
{
	Super::SetSpatialObservationActive_Internal(bActive);
	if (bActive)
	{
		if (Configs.IsEmpty())
		{
			for (int i = 0; i < Settings->Configs.Num(); i++)
			{
				if (Settings->Configs[i].IsValid())
				{
					Configs.Add(i, FConfigExecutor(this, i));
					ConfigsQueue.Add(i);
				}
			}
		}
		else
		{
			ConfigsQueue.Reset();
			for (auto& Config : Configs)
			{
				Config.Value.Restart();
				ConfigsQueue.Add(Config.Key);
			}
		}
	}
}

void USpatialObservationComponent_GoodBoy::ResetRaindropData()
{
	Super::ResetRaindropData();
	Configs.Empty();
	ConfigsQueue.Reset();
}

void USpatialObservationComponent_GoodBoy::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(USpatialObservationComponent_GoodBoy::TickComponent)	
	
		if (!ensure(bSpatialObservationActive))
		{
			SetComponentTickEnabled(false);
			return;
		}
		
		if (Configs.IsEmpty() || ConfigsQueue.IsEmpty())
		{
			SetComponentTickEnabled(false); // waiting for cooldown to end for some config
			return;
		}
		
		UE_LOG(LogNpcMl_Raindrop_Debug, Log, TEXT("Tick: frame %llu START"), GFrameCounter);
		
		int TracesLeft = GetMaxCountOfRequests();
		TArray<int, TInlineAllocator<4>> ConfigsQueueCopy = ConfigsQueue;
		for (const int ConfigIndex : ConfigsQueueCopy)
		{
			auto& Config = Configs[ConfigIndex];
			TracesLeft -= Config.RequestAsyncTraces(TracesLeft);
			if (Config.IsCompleted())
			{
				ConfigsQueue.Remove(ConfigIndex);
#if WITH_EDITOR
				TWeakObjectPtr ThisWeak = this;
				GetWorld()->GetTimerManager().SetTimerForNextTick([ThisWeak, ConfigIndex]()
				{
					if (!ThisWeak.IsValid())
						return;
					
					auto EndTime = FPlatformTime::Seconds();
					auto ConfigInfo = ThisWeak->Settings->Configs[ConfigIndex];
					auto ConfigQueueData = ThisWeak->Configs[ConfigIndex];
					auto Duration = EndTime - ConfigQueueData.RequestedAt;
					UE_LOG(LogNpcMl_Raindrop, Log, TEXT("Frame %llu:: Finished traces for config %d (%s) in %.2f ms"), GFrameCounter, ConfigIndex, *ConfigInfo.UserDescription, Duration * 1000);
					UE_LOG(LogNpcMl_Raindrop, Log, TEXT("Traces count: %d"), ThisWeak->Configs[ConfigIndex].GetTotalTracesCount());
					
				});
#endif
			}
			
			if (TracesLeft == 0)
				break;
		}

#if WITH_EDITOR
		if (bDebug_DoOnce && ConfigsQueue.IsEmpty())
		{
			SetSpatialObservationActive(false);
			bDebug_DoOnce = false;
		}
		
		UE_LOG(LogNpcMl_Raindrop_Debug, Log, TEXT("Tick: frame %llu END"), GFrameCounter);
#endif
	}
}

void USpatialObservationComponent_GoodBoy::Debug_DoOnce()
{
	bDebug_DoOnce = true;
	SetSpatialObservationActive(true);
}

void USpatialObservationComponent_GoodBoy::RestartQueue(int ConfigIndex)
{
	Configs[ConfigIndex].Restart();
	ConfigsQueue.Add(ConfigIndex);
	if (bSpatialObservationActive && !IsComponentTickEnabled())
		SetComponentTickEnabled(true);
}

int USpatialObservationComponent_GoodBoy::GetMaxCountOfRequests() const
{
	// 7 Mar 2026: (aki) TODO:
	// instead of a hardcoded value, make a subsystem that has a global pool of "tickets" for async traces and provide them to agents evenly
	// e.g. there can be a total of 1000 async trace tickets. but if there are only 2 agents - each gets 500 async requests
	// if there are 8 agents, each gets 125 tickets per tick
	return MaxCountOfRequests;
}

void USpatialObservationComponent_GoodBoy::AsyncTraceCallback(const FTraceHandle& TraceHandle, FTraceDatum& TraceDatum)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(USpatialObservationComponent_GoodBoy::AsyncTraceCallback)
	UE_LOG(LogNpcMl_Raindrop_Debug, Log, TEXT("AsyncTraceCallback: frame %llu"), GFrameCounter);
	
	const int ConfigIndex = (TraceDatum.UserData & ConfigIndexMask) >> ConfigIndexMaskShift;
	const int GridIndex = (TraceDatum.UserData & GridIndexMask) >> GridIndexMaskShift;
	const int CellSpan = (TraceDatum.UserData & CellSpanMask) >> CellSpanMaskShift;
	const int ElementIndex = TraceDatum.UserData & ElementIndexMask;
	const auto& Params = Settings->Configs[ConfigIndex].Params;
	const int Rows = Params.Rows;
	const int Columns = Params.Columns;
	const int TraceRowIndex = ElementIndex / Columns;
	const int TraceColumnIndex = ElementIndex % Columns;

	UE_LOG(LogNpcMl_Raindrop_Debug, Log, TEXT("AsyncTraceCallback [%d, %d]: Row = %d, Column = %d"), ConfigIndex, GridIndex, TraceRowIndex, TraceColumnIndex);
	
	const float ShapeSize = Params.SweepShapeScale * Params.CellDimension / 2.f * Settings->Configs[ConfigIndex].Grids[GridIndex].CellSpan; 
#if WITH_EDITOR
	FRaindropDebugData_Trace DebugTrace;
	DebugTrace.TraceStart = TraceDatum.Start;
	DebugTrace.Orientation = TraceDatum.Rot;
	DebugTrace.TraceEnd = TraceDatum.End;
	DebugTrace.ShapeSize = ShapeSize;
#endif
	
	float TraceDistance = 0.f;
	if (!TraceDatum.OutHits.IsEmpty())
	{
		ensure(TraceDatum.OutHits[0].bBlockingHit);
		TraceDistance = TraceDatum.OutHits[0].Distance;
#if WITH_EDITOR
		DebugTrace.TraceEnd = TraceDatum.OutHits[0].Location;
#endif
	}
	else
	{
		TraceDistance = (TraceDatum.End - TraceDatum.Start).Size();
	}

	float ExtraDistance = Params.TraceMode == ERaindropTraceMode::SphereSweep ? ShapeSize : 0.f;
	float RawValue = (TraceDistance + ExtraDistance) / Params.TraceDistance;
	float UnifiedValue = FMath::RoundToFloat(RawValue * 1000000.f) / 1000000.f;
	auto GridArrayView = RaindropData[ConfigIndex].GetArrayView(GridIndex);
	
	if (CellSpan == 1)
	{
		GridArrayView[ElementIndex] = UnifiedValue;
	}
	else
	{
		for (int i = 0; i < CellSpan && TraceRowIndex + i < Rows; i++)
			for (int j = 0; j < CellSpan && TraceColumnIndex + j < Columns; j++)
				GridArrayView[(TraceRowIndex + i) * Columns + TraceColumnIndex + j] = UnifiedValue;
	}
	
#if WITH_EDITOR
	const auto& GridConfig = Settings->Configs[ConfigIndex].Grids[GridIndex];
	if (GridConfig.Debug_DrawGridShapes)
		Configs[ConfigIndex][GridIndex].DebugData.Traces.Add(DebugTrace);
#endif
}

USpatialObservationComponent_GoodBoy::FConfigExecutor::FConfigExecutor(USpatialObservationComponent_GoodBoy* InOwner, int InConfigIndex)
	: Owner(InOwner), ConfigIndex(InConfigIndex)
{
	for (int i = 0; i < Owner->Settings->Configs[ConfigIndex].Grids.Num(); i++)
		GridQueue.Add(FGridExecutor(InOwner, ConfigIndex, i));
	
	RestartTimerDelegate = FTimerDelegate::CreateUObject(Owner.Get(), &USpatialObservationComponent_GoodBoy::RestartQueue, ConfigIndex);
	RequestedAt = FPlatformTime::Seconds();
}

int USpatialObservationComponent_GoodBoy::FConfigExecutor::RequestAsyncTraces(int TracesBudget)
{
	ensure(!bCompleted);
	int TracesDone = 0;
	while (TracesDone < TracesBudget && CurrentGridIndex < GridQueue.Num())
	{
		TracesDone += GridQueue[CurrentGridIndex].RequestAsyncTraces(TracesBudget - TracesDone);
		if (GridQueue[CurrentGridIndex].IsCompleted())
			CurrentGridIndex++;
	}

	bCompleted = CurrentGridIndex == GridQueue.Num();
	if (bCompleted)
	{
		const float UpdateInterval = Owner->Settings->Configs[ConfigIndex].UpdateInterval;
		Owner->GetWorld()->GetTimerManager().SetTimer(RestartQueueTimer, RestartTimerDelegate, UpdateInterval, false);
	}
	
	ensure(TracesBudget - TracesDone >= 0);
	return TracesDone;
}

void USpatialObservationComponent_GoodBoy::FConfigExecutor::Restart()
{
	for (int i = 0; i < GridQueue.Num(); i++)
		GridQueue[i].Restart();

	RequestedAt = FPlatformTime::Seconds();
	CurrentGridIndex = 0;
	bCompleted = false;
}

USpatialObservationComponent_GoodBoy::FGridExecutor::FGridExecutor(USpatialObservationComponent_GoodBoy* InOwner, int InConfigIndex, int InGridIndex) 
	: Owner(InOwner), ConfigIndex(InConfigIndex), GridIndex(InGridIndex)
{
	const auto& GridDescriptor = InOwner->Settings->Configs[ConfigIndex].Grids[GridIndex];
	TraceData |= (ConfigIndex << ConfigIndexMaskShift) & ConfigIndexMask;
	TraceData |= (GridIndex << GridIndexMaskShift) & GridIndexMask;
	TraceData |= (GridDescriptor.CellSpan << CellSpanMaskShift) & CellSpanMask;
#if WITH_EDITOR
	DebugData = FRaindropGridDebugData(Owner->Settings->Configs[ConfigIndex].Params, FPlatformTime::Seconds(), 
		GridDescriptor.CellSpan, GridDescriptor.Debug_DrawGridShapes);
#endif
}

int USpatialObservationComponent_GoodBoy::FGridExecutor::RequestAsyncTraces(int TracesBudget)
{
	if (!ensure(TracesBudget > 0))
		return 0;
	
	if (LastRow == 0 && LastColumn == 0)
	{
	#if WITH_EDITOR
		DebugData.StartTime = FPlatformTime::Seconds();		
	#endif
		Variables = Owner->GetVariables(ConfigIndex, GridIndex);
	}
	
	int CountOfTraces = 0;
	auto Params = Owner->Settings->Configs[ConfigIndex].Params;
	auto WorldLocal = Owner->GetWorld();
	
	int CurrentRow = LastRow;
	int CurrentColumn = LastColumn;
	ensure(CurrentRow < Params.Rows);
	
	for (; CurrentRow < Params.Rows; CurrentRow += Variables.CellSpan)
	{
		const FVector StartPoint = Variables.OriginLocation 
			+ (-Variables.RightVector * Variables.BaseOffsetH)  
			+ (Variables.UpVector * (Variables.BaseOffsetV - CurrentRow * Params.CellDimension));
		
		if (CurrentColumn >= Params.Columns)
			CurrentColumn = 0;
		
		for (; CurrentColumn < Params.Columns && CountOfTraces < TracesBudget; CurrentColumn += Variables.CellSpan)
		{
			const FVector TraceStart = StartPoint + Variables.RightVector * CurrentColumn * Params.CellDimension;
			const FVector TraceEnd = TraceStart + Variables.TraceDirection * Variables.TraceDistance;
			FTraceHandle TraceHandle;
			TraceData = (TraceData & ~ElementIndexMask) | ((CurrentRow * Params.Columns + CurrentColumn) & ElementIndexMask);
#if WITH_EDITOR
				const int ElementIndex = TraceData & ElementIndexMask;
				const int CheckRowIndex = ElementIndex / Params.Columns;
				const int CheckColumnIndex = ElementIndex % Params.Columns;
				ensure(CheckRowIndex == CurrentRow);
				ensure(CheckColumnIndex == CurrentColumn);
#endif
				
			switch (Params.TraceMode)
			{
				case ERaindropTraceMode::Linetrace:
					TraceHandle = WorldLocal->AsyncLineTraceByChannel(EAsyncTraceType::Single, TraceStart, TraceEnd, 
						Params.TraceChannel, Owner->CollisionQueryParams, FCollisionResponseParams::DefaultResponseParam,
						&Owner->TraceDelegate, TraceData);
					break;
				case ERaindropTraceMode::BoxSweep:
				case ERaindropTraceMode::SphereSweep:
					TraceHandle = WorldLocal->AsyncSweepByChannel(EAsyncTraceType::Single, TraceStart, TraceEnd, 
						Variables.SweepRotation, Params.TraceChannel, Variables.SweepShape, Owner->CollisionQueryParams,
						FCollisionResponseParams::DefaultResponseParam,
						&Owner->TraceDelegate, TraceData);
					break;
			}
			
			if (ensure(TraceHandle.IsValid()))
			{
				CountOfTraces++;
#if WITH_EDITOR
				DebugData.TracesCount++;
				UE_LOG(LogNpcMl_Raindrop_Debug, Log, TEXT("Requested async trace: Grid = %d, Row = %d, Column = %d"), GridIndex, CurrentRow, CurrentColumn);
#endif
			}
			else
			{
				return CountOfTraces;
			}
				// idk it probably means that no more traces can be requested (although i'm not sure there's a code for that down the async trace calls.
		}
		
		// must be in the end of the loop body so that LastRow gets a correct value for continuation
		if (CountOfTraces >= TracesBudget)
		{
			if (CurrentColumn >= Params.Columns)
				CurrentRow++;
			
			break;
		}
	}
	
	LastRow = CurrentRow;
	LastColumn = CurrentColumn;
	
	bool bGridProcessed = CurrentRow >= Params.Rows && CurrentColumn >= Params.Columns;
	if (bGridProcessed)
	{
		bCompleted = true;
#if WITH_EDITOR
		TWeakObjectPtr OwnerWeak = Owner;
		Owner->GetWorld()->GetTimerManager().SetTimerForNextTick([OwnerWeak, ConfigIndex = ConfigIndex, GridIndex = GridIndex]()
		{ 
			if (OwnerWeak.IsValid())
			{
				OwnerWeak->Configs[ConfigIndex][GridIndex].DebugData.FinishedAt = FPlatformTime::Seconds();
				OwnerWeak->GetWorld()->GetTimerManager().SetTimerForNextTick([OwnerWeak, ConfigIndex, GridIndex]()
				{
					if (OwnerWeak.IsValid())
					{
						OwnerWeak->ProcessRaindropDebug(OwnerWeak->Configs[ConfigIndex][GridIndex].DebugData,
							OwnerWeak->RaindropData[ConfigIndex].GetArrayView(GridIndex), ConfigIndex, GridIndex);
					}
				});
			}
		});
#endif
	}

	return CountOfTraces;
}

void USpatialObservationComponent_GoodBoy::FGridExecutor::Restart()
{
	LastRow = 0;
	LastColumn = 0;
	bCompleted = false;
#if WITH_EDITOR
	DebugData.Reset();
	DebugData.RequestedAt = FPlatformTime::Seconds();
#endif
}

#if WITH_EDITOR

int USpatialObservationComponent_GoodBoy::FConfigExecutor::GetTotalTracesCount() const
{
	int Result = 0;
	for (const auto& Grid : GridQueue)
		Result += Grid.DebugData.TracesCount;
	
	return Result;
}

#endif
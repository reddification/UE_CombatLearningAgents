#include "Components/SpatialObservationComponent_GoodBoy.h"

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
	CurrentConfigIndex = GetNextConfigIndex(0);
}

void USpatialObservationComponent_GoodBoy::SetSpatialObservationActive_Internal(bool bActive)
{
	Super::SetSpatialObservationActive_Internal(bActive);
#if WITH_EDITOR
	if (bActive)
	{
		GridsDebugDataMap.Empty();
		for (int i = 0; i < Settings->Configs.Num(); i++)
		{
			if (Settings->Configs[i].IsValid())
			{
				GridsDebugDataMap.Add(i);
				for (int g = 0; g < Settings->Configs[i].Grids.Num(); g++)
					GridsDebugDataMap[i].Add(FRaindropGridDebugData());
			}
		}
	}
#endif
}

void USpatialObservationComponent_GoodBoy::ResetRaindropData()
{
	Super::ResetRaindropData();
#if WITH_EDITOR
	GridsDebugDataMap.Reset();
#endif
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
	
	if (CurrentConfigIndex < 0)
	{
		SetComponentTickEnabled(false); // waiting for cooldown to end for some config
		return;
	}
	
	const int MaxCountOfRequestsThisFrame = GetMaxCountOfRequests();
	
	const bool bNewConfig = CurrentGridIndex == 0 && RowIndex == 0 && ColumnIndex == 0;
	if (bNewConfig)
	{
		auto TimeNow =  FPlatformTime::Seconds();
		Variables = GetVariables(CurrentConfigIndex, CurrentGridIndex);
		RaindropSchedules[CurrentConfigIndex].StartTime = TimeNow;
#if WITH_EDITOR
		GridsDebugDataMap[CurrentConfigIndex][CurrentGridIndex] = FRaindropGridDebugData(Settings->Configs[CurrentConfigIndex].Params, TimeNow);
#endif
	}
	
	const FRaindropParams& Params = Settings->Configs[CurrentConfigIndex].Params;
	const FCollisionResponseParams& CollisionResponseParams = FCollisionResponseParams::DefaultResponseParam;
	int CountOfTraces = 0;
	auto WorldLocal = GetWorld();
	while (CountOfTraces < MaxCountOfRequestsThisFrame)
	{
		uint32 TraceData = 0;
		TraceData |= (CurrentConfigIndex << ConfigIndexMaskShift) & ConfigIndexMask;
		TraceData |= (CurrentGridIndex << GridIndexMaskShift) & GridIndexMask;
		TraceData |= (Variables.CellSpan << CellSpanMaskShift) & CellSpanMask;
		
#if WITH_EDITOR
		const int CheckConfigIndex = (TraceData & ConfigIndexMask) >> ConfigIndexMaskShift;
		const int CheckGridIndex = (TraceData & GridIndexMask) >> GridIndexMaskShift;
		const int CheckCellSpan = (TraceData & CellSpanMask) >> CellSpanMaskShift;
		ensure(CheckConfigIndex == CurrentConfigIndex);
		ensure(CheckGridIndex == CurrentGridIndex);
		ensure(CheckCellSpan == Variables.CellSpan);
#endif
		
		for (; RowIndex < Params.Rows && CountOfTraces < MaxCountOfRequestsThisFrame; RowIndex += Variables.CellSpan)
		{
			const FVector StartPoint = Variables.OriginLocation 
				+ (-Variables.RightVector * Variables.BaseOffsetH)  
				+ (Variables.UpVector * (Variables.BaseOffsetV - RowIndex * Params.CellDimension));
			
			for (; ColumnIndex < Params.Columns && CountOfTraces < MaxCountOfRequestsThisFrame; ColumnIndex += Variables.CellSpan)
			{
				const FVector TraceStart = StartPoint + Variables.RightVector * ColumnIndex * Params.CellDimension;
				const FVector TraceEnd = TraceStart + Variables.TraceDirection * Variables.TraceDistance;
				FTraceHandle TraceHandle;
				TraceData = (TraceData & ~ElementIndexMask) | ((RowIndex * Params.Columns + ColumnIndex) & ElementIndexMask);
#if WITH_EDITOR
				const int ElementIndex = TraceData & ElementIndexMask;
				const int CheckRowIndex = ElementIndex / Params.Columns;
				const int CheckColumnIndex = ElementIndex % Params.Columns;
				ensure(CheckRowIndex == RowIndex);
				ensure(CheckColumnIndex == ColumnIndex);
#endif
				
				switch (Params.TraceMode)
				{
					case ERaindropTraceMode::Linetrace:
						TraceHandle = WorldLocal->AsyncLineTraceByChannel(EAsyncTraceType::Single, TraceStart, TraceEnd, 
							Params.TraceChannel, CollisionQueryParams, CollisionResponseParams,
							&TraceDelegate, TraceData);
						break;
					case ERaindropTraceMode::BoxSweep:
					case ERaindropTraceMode::SphereSweep:
						TraceHandle = WorldLocal->AsyncSweepByChannel(EAsyncTraceType::Single, TraceStart, TraceEnd, 
							Variables.SweepRotation, Params.TraceChannel, Variables.SweepShape, CollisionQueryParams, CollisionResponseParams,
							&TraceDelegate, TraceData);
						break;
				}
				
				if (ensure(TraceHandle.IsValid()))
					CountOfTraces++;
				else 
					return; // idk it probably means that no more traces can be requested (although i'm not sure there's a code for that down the async trace calls.
			}
			
		}
		
		bool bGridProcessed = RowIndex + Variables.CellSpan >= Params.Rows && ColumnIndex + Variables.CellSpan >= Params.Columns;
		if (bGridProcessed)
		{
			OnGridLastTraceRequested(DeltaTime);
			if (CurrentConfigIndex >= 0)
				Variables = GetVariables(CurrentConfigIndex, CurrentGridIndex);
			else 
				break;
		}
	}
	}
}

void USpatialObservationComponent_GoodBoy::Debug_DoOnce()
{
	bDebug_DoOnce = true;
	SetSpatialObservationActive(true);
}

void USpatialObservationComponent_GoodBoy::RemoveCooldown(int ConfigIndex)
{
	RaindropSchedules[ConfigIndex].Reset(GetWorld());
	if (CurrentConfigIndex == -1 && bSpatialObservationActive)
	{
		CurrentConfigIndex = GetNextConfigIndex(0);
		if (ensure(CurrentConfigIndex >= 0))
			SetComponentTickEnabled(true);
	}
}

void USpatialObservationComponent_GoodBoy::OnGridLastTraceRequested(const float DeltaTime)
{
	
#if WITH_EDITOR
	const auto TimeNow = FPlatformTime::Seconds();
	// + delta time because data will be ready on next tick and I assume that frame rate is stable
	GridsDebugDataMap[CurrentConfigIndex][CurrentGridIndex].FinishedAt = TimeNow + DeltaTime; 
	// when last request for grid was made - debug data will only be ready in next 2 ticks with certainty
	TWeakObjectPtr ThisWeak = this;
	GetWorld()->GetTimerManager().SetTimerForNextTick([ThisWeak, ConfigIndex = CurrentConfigIndex, GridIndex = CurrentGridIndex]()
	{ 
		if (ThisWeak.IsValid())
		{
			ThisWeak->GetWorld()->GetTimerManager().SetTimerForNextTick([ThisWeak, ConfigIndex, GridIndex]()
			{
				if (ThisWeak.IsValid())
					ThisWeak->ProcessRaindropDebug(ThisWeak->GridsDebugDataMap[ConfigIndex][GridIndex], ThisWeak->RaindropConfigsData[ConfigIndex].GetArrayView(GridIndex), ConfigIndex, GridIndex);
			});
		}
	});
#endif
	
	RaindropSchedules[CurrentConfigIndex].IncrementGridsCompleted();
	if (RaindropSchedules[CurrentConfigIndex].IsCompleted())
	{
		FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &USpatialObservationComponent_GoodBoy::RemoveCooldown, CurrentConfigIndex);
		GetWorld()->GetTimerManager().SetTimer(RaindropSchedules[CurrentConfigIndex].CooldownTimer, Delegate,  RaindropSchedules[CurrentConfigIndex].UpdateInterval, false);
		CurrentConfigIndex = GetNextConfigIndex(CurrentConfigIndex);
		if (CurrentConfigIndex >= 0)
			CurrentGridIndex = 0;
	}
	else
	{
		CurrentGridIndex = (CurrentGridIndex + 1) % RaindropConfigsData[CurrentConfigIndex].GridsCount;
	}

#if WITH_EDITOR
	if (CurrentConfigIndex >= 0)
	{
		GridsDebugDataMap[CurrentConfigIndex][CurrentGridIndex] = FRaindropGridDebugData(Settings->Configs[CurrentConfigIndex].Params, TimeNow);
		GridsDebugDataMap[CurrentConfigIndex][CurrentGridIndex].StartTime = TimeNow;
	}
	else if (bDebug_DoOnce)
	{
		SetSpatialObservationActive(false);
		bDebug_DoOnce = false;
	}
#endif
	
	RowIndex = 0;
	ColumnIndex = 0;
}

int USpatialObservationComponent_GoodBoy::GetNextConfigIndex(int StartingFrom)
{
	const int ConfigsNum = Settings->Configs.Num();
	for (int i = 0; i < ConfigsNum; i++)
	{
		int TestIndex = (StartingFrom + i) % ConfigsNum;
		if (Settings->Configs[TestIndex].IsValid() && !RaindropSchedules[TestIndex].IsOnCooldown())
			return TestIndex;
	}
	
	return -1;
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
	
	const int ConfigIndex = (TraceDatum.UserData & ConfigIndexMask) >> ConfigIndexMaskShift;
	const int GridIndex = (TraceDatum.UserData & GridIndexMask) >> GridIndexMaskShift;
	const int CellSpan = (TraceDatum.UserData & CellSpanMask) >> CellSpanMaskShift;
	const int ElementIndex = TraceDatum.UserData & ElementIndexMask;
	const auto& Params = Settings->Configs[ConfigIndex].Params;
	const int Rows = Params.Rows;
	const int Columns = Params.Columns;
	const int TraceRowIndex = ElementIndex / Columns;
	const int TraceColumnIndex = ElementIndex % Columns;
	
#if WITH_EDITOR
	FRaindropDebugData_Trace DebugTrace;
	DebugTrace.TraceStart = TraceDatum.Start;
	DebugTrace.Orientation = TraceDatum.Rot;
	DebugTrace.TraceEnd = TraceDatum.End;
	DebugTrace.ShapeSize = Params.SweepShapeScale * Params.CellDimension / 2.f * Settings->Configs[ConfigIndex].Grids[GridIndex].CellSpan;
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

	float RawValue = TraceDistance / Settings->Configs[ConfigIndex].Params.TraceDistance;
	float UnifiedValue = FMath::RoundToFloat(RawValue * 100.f) / 100.f;
	auto GridArrayView = RaindropConfigsData[ConfigIndex].GetArrayView(GridIndex);
	
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
	if (Settings->Configs[ConfigIndex].Grids[GridIndex].Debug_DrawGridShapes)
		GridsDebugDataMap[ConfigIndex][GridIndex].Traces.Add(DebugTrace);
#endif
}
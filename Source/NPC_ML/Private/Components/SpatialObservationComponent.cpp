#include "Components/SpatialObservationComponent.h"

#include "Data/LogChannels.h"
#include "Settings/CombatLearningSettings.h"
#include "Settings/RaindropSettings.h"

USpatialObservationComponent::USpatialObservationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USpatialObservationComponent::BeginPlay()
{
	Super::BeginPlay();
	CollisionQueryParams.AddIgnoredActor(GetOwner());
	Settings = GetDefault<URaindropSettings>();
}

void USpatialObservationComponent::SetSpatialObservationActive(bool bActive)
{
	if (bSpatialObservationActive == bActive)
	{
		UE_VLOG(GetOwner(), LogNpcMl_Raindrop, Warning, TEXT("USpatialObservationComponent::SetSpatialObservationActive: same state requested: %d"), bActive ? 1 : 0);
		return;
	}
	
	SetSpatialObservationActive_Internal(bActive);
}

void USpatialObservationComponent::SetSpatialObservationActive_Internal(bool bActive)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(USpatialObservationComponent::SetSpatialObservationActive_Internal);
	bSpatialObservationActive = bActive;
	SetComponentTickEnabled(bActive);
	
	if (ResetRaindropBuffersTimer.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(ResetRaindropBuffersTimer);
	
	if (bActive)
	{
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
	}
	else
	{
		// 2 seconds should be enough for all raindrops to stop
		GetWorld()->GetTimerManager().SetTimer(ResetRaindropBuffersTimer, this, &USpatialObservationComponent::ResetRaindropData, 2.f);
	}
}

FRaindropVariables USpatialObservationComponent::GetVariables(int ConfigIndex, int GridIndex) const
{
	const auto& GridDescriptor = Settings->Configs[ConfigIndex].Grids[GridIndex];
	const auto& ConfigParams = Settings->Configs[ConfigIndex].Params;
	auto OwnerLocal = GetOwner();
	FVector FV = OwnerLocal->GetActorForwardVector();
	FVector Origin = OwnerLocal->GetActorLocation() + FV.Rotation().RotateVector(GridDescriptor.Offset);
	FQuat ActorQuat = OwnerLocal->GetActorQuat();
	if (!GridDescriptor.Offset.IsNearlyZero())
	{
		UE_VLOG_LOCATION(OwnerLocal, LogNpcMl_Raindrop, Verbose, OwnerLocal->GetActorLocation(), 12, FColor::White, TEXT("Actor location"));
		UE_VLOG_LOCATION(OwnerLocal, LogNpcMl_Raindrop, Verbose, Origin, 12, FColor::Yellow, TEXT("Actual grid pivot"));
	}
	
	FVector ForwardVector = OwnerLocal->GetActorForwardVector(); 
	FVector UpVector = OwnerLocal->GetActorUpVector();
	FVector RightVector;
	if (!GridDescriptor.Rotation.IsNearlyZero())
	{
		FQuat FinalRotation = ActorQuat * GridDescriptor.Rotation.Quaternion();
		ForwardVector = FinalRotation.GetForwardVector();
		UpVector = FinalRotation.GetUpVector();
	}
	
	RightVector = FVector::CrossProduct(UpVector, ForwardVector);
		
	FRaindropVariables Variables;
	Variables.OriginLocation = Origin;
	Variables.TraceDirection = ForwardVector;
	Variables.UpVector = UpVector;
	Variables.RightVector = RightVector;
	Variables.TraceDistance = ConfigParams.TraceDistance;
	Variables.CellSpan = GridDescriptor.CellSpan;
	Variables.CellCenterOffset = ConfigParams.CellDimension * Variables.CellSpan / 2.f;
	Variables.BaseOffsetH = ConfigParams.CellDimension * ConfigParams.Columns / 2.f - Variables.CellCenterOffset;
	Variables.BaseOffsetV = ConfigParams.CellDimension * ConfigParams.Rows / 2.f - Variables.CellCenterOffset;
	// Variables.SweepRotation = Variables.TraceDirection.ToOrientationQuat();
	Variables.SweepRotation = FRotationMatrix::MakeFromXZ(Variables.TraceDirection,Variables.UpVector).ToQuat();
#if WITH_EDITOR
	Variables.bNeedsTraces = GridDescriptor.Debug_DrawGridShapes;
#endif
	switch (ConfigParams.TraceMode)
	{
		case ERaindropTraceMode::Linetrace:
			break;
		case ERaindropTraceMode::BoxSweep:
			Variables.SweepShape = FCollisionShape::MakeBox(FVector(5.f, 
				Variables.CellCenterOffset * ConfigParams.SweepShapeScale, Variables.CellCenterOffset * ConfigParams.SweepShapeScale));
			break;
		case ERaindropTraceMode::SphereSweep:
			float Radius = Variables.CellCenterOffset * ConfigParams.SweepShapeScale;
			Variables.SweepShape = FCollisionShape::MakeSphere(Radius);
			Variables.OriginLocation += Variables.TraceDirection * Radius;
			break;
	}
	
	return Variables;
}

void USpatialObservationComponent::ResetRaindropData()
{
	RaindropData.Empty();
}

#if WITH_EDITOR

void USpatialObservationComponent::ProcessRaindropDebug(const FRaindropGridDebugData& DebugData, const TArrayView<const float>& RaindropDataSlice,
                                                        int ConfigIndex, int GridIndex) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(USpatialObservationComponent_Stud::ProcessRaindropDebug);
	
	const double EndTime = FPlatformTime::Seconds();
	auto LogOwner = GetOwner();
	const FString& ConfigInfo = Settings->Configs[ConfigIndex].UserDescription;
	const auto& Grid = Settings->Configs[ConfigIndex].Grids[GridIndex];
	const FString& GridInfo = Grid.UserDescription;
	
	UE_VLOG_UELOG(LogOwner, LogNpcMl_Raindrop_Debug, Log, 
		TEXT("Raindrop grid [%s -> %s] completed. Total Duration = %.2f ms; Background work duration = %.2f ms. Frame: %llu"),
		*ConfigInfo, *GridInfo, (EndTime - DebugData.RequestedAt) * 1000.0, (DebugData.FinishedAt - DebugData.StartTime) * 1000.0, GFrameCounter);
					
	if (Grid.Debug_DrawGridShapes)
	{
		auto WorldLocal = LogOwner->GetWorld();
		for (const auto& DebugTrace : DebugData.Traces)
		{
			switch (DebugData.TraceMode) 
			{
			case ERaindropTraceMode::Linetrace:
				DrawDebugLine(WorldLocal, DebugTrace.TraceStart, DebugTrace.TraceEnd, FColor::Green,
					false, DebugDrawDuration, 0, 1.f);
				DrawDebugBox(WorldLocal, DebugTrace.TraceEnd, FVector(5.f), FColor::Red, 
					false, DebugDrawDuration, 0, 1.f);
				break;
			case ERaindropTraceMode::BoxSweep:
				{
					FVector BoxCenter = (DebugTrace.TraceStart + DebugTrace.TraceEnd) / 2.f;
					FVector Extent = FVector((DebugTrace.TraceEnd - DebugTrace.TraceStart).Size() / 2, DebugTrace.ShapeSize * 2.f,
						DebugTrace.ShapeSize * 2.f);
					DrawDebugBox(WorldLocal, BoxCenter, Extent, DebugTrace.Orientation, FColor::Green, 
						false, DebugDrawDuration, 0, 1.f);
				}
				break;
			case ERaindropTraceMode::SphereSweep:
				{
					FVector CapsuleCenter = (DebugTrace.TraceStart + DebugTrace.TraceEnd) / 2.f;
					float HalfHeight = (DebugTrace.TraceEnd - DebugTrace.TraceStart).Size() / 2.f;
					FRotator ForDebug = DebugTrace.Orientation.Rotator();
					FRotator WorldUpRotated = DebugTrace.Orientation.RotateVector(FVector::UpVector).Rotation();
					FQuat CapsuleRotation = FRotator(WorldUpRotated.Pitch, ForDebug.Yaw, 0).Quaternion();
					DrawDebugCapsule(WorldLocal, CapsuleCenter, HalfHeight, DebugTrace.ShapeSize, CapsuleRotation, FColor::Green, 
						false, DebugDrawDuration, 0, 1.f);
				}
				break;
			default:
				ensure(false);
			}
		}
	}
	
	if (Grid.Debug_DumpGridToLog)
	{
		UE_LOG(LogNpcMl_Raindrop, Log, TEXT("Raindrop %s->%s dump:"), *ConfigInfo, *GridInfo);
		FString GridLogLine;
		GridLogLine.Reserve(DebugData.Rows * 5); // "x.xx " -> 5 characters
		for (int i = 0; i < DebugData.Rows; i++)
		{
			GridLogLine.Reset();
			for (int j = 0; j < DebugData.Columns; j++)
				GridLogLine += FString::Printf(TEXT("%.2f "), RaindropDataSlice[i * DebugData.Columns + j]);
		
			UE_LOG(LogNpcMl_Raindrop, Log, TEXT("%s"), *GridLogLine);
		}
	}
}

void USpatialObservationComponent::Debug_SpawnActorInfront()
{
	if (!IsValid(StressTest_SpawnActorClass))
		return;
	
	FVector SpawnLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 250.f;
	auto SpawnedActor = GetWorld()->SpawnActor(StressTest_SpawnActorClass, &SpawnLocation);
}

#endif

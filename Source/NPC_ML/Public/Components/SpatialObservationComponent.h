// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/RaindropDataTypes.h"
#include "SpatialObservationComponent.generated.h"

class URaindropSettings;
class UCombatLearningSettings;

// raindrop approach is probably the most accurate, but also the least scalable
// I can only excuse myself by saying that 
// 1. it's async and won't stagger the game thread
// 2. it's ok if it takes 100-150ms to complete for an agent because real humans also don't process the information immediately
// that said, still TODO find a proper octree implementation and use it instead
UCLASS(Abstract)
class NPC_ML_API USpatialObservationComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

#if WITH_EDITORONLY_DATA
	
	struct FRaindropDebugData_Arrow
	{
		FVector Location = FVector::ZeroVector;
		FVector Direction = FVector::ZeroVector;
		FString Text;
	};
	
	struct FRaindropDebugData_Trace
	{
		FVector TraceStart = FVector::ZeroVector;
		FVector TraceEnd = FVector::ZeroVector;
		FQuat Orientation = FQuat::Identity;
		float ShapeSize = 0.f;
	};
	
	struct FRaindropGridDebugData
	{
		FRaindropGridDebugData() = default;
		
		FRaindropGridDebugData(const FRaindropGridDebugData& Other)
			: Arrows(Other.Arrows),
			  Traces(Other.Traces),
			  RequestedAt(Other.RequestedAt),
			  StartTime(Other.StartTime),
			  FinishedAt(Other.FinishedAt),
			  CellsCountPerRow(Other.CellsCountPerRow),
			  TraceMode(Other.TraceMode)
		{
		}

		FRaindropGridDebugData(FRaindropGridDebugData&& Other) noexcept
			: Arrows(std::move(Other.Arrows)),
			  Traces(std::move(Other.Traces)),
			  RequestedAt(Other.RequestedAt),
			  StartTime(Other.StartTime),
			  FinishedAt(Other.FinishedAt),
			  CellsCountPerRow(Other.CellsCountPerRow),
			  TraceMode(Other.TraceMode)
		{
		}

		FRaindropGridDebugData(const FRaindropParams& RaindropParams, double InRequestedAt)
		{
			RequestedAt = InRequestedAt;
			StartTime = FPlatformTime::Seconds();
			TraceMode = RaindropParams.TraceMode;
			CellsCountPerRow = RaindropParams.Rows;
			CellsCountPerColumn = RaindropParams.Columns;
		};

		FRaindropGridDebugData& operator=(const FRaindropGridDebugData& Other)
		{
			if (this == &Other)
				return *this;
			
			Arrows = Other.Arrows;
			Traces = Other.Traces;
			RequestedAt = Other.RequestedAt;
			StartTime = Other.StartTime;
			FinishedAt = Other.FinishedAt;
			CellsCountPerRow = Other.CellsCountPerRow;
			TraceMode = Other.TraceMode;
			return *this;
		}

		FRaindropGridDebugData& operator=(FRaindropGridDebugData&& Other) noexcept
		{
			if (this == &Other)
				return *this;
			
			Arrows = std::move(Other.Arrows);
			Traces = std::move(Other.Traces);
			RequestedAt = Other.RequestedAt;
			StartTime = Other.StartTime;
			FinishedAt = Other.FinishedAt;
			CellsCountPerRow = Other.CellsCountPerRow;
			TraceMode = Other.TraceMode;
			return *this;
		}

		void Reset()
		{
			Arrows.Empty();
			Traces.Empty();
		};

		TArray<FRaindropDebugData_Arrow> Arrows;
		TArray<FRaindropDebugData_Trace> Traces;
		double RequestedAt = 0;
		double StartTime = 0;
		double FinishedAt = 0;
		int CellsCountPerRow = 0;
		int CellsCountPerColumn = 0;
		ERaindropTraceMode TraceMode = ERaindropTraceMode::Linetrace;
	};
	
#endif

	struct FRaindropCategorySchedule
	{
		FRaindropCategorySchedule(int InTotalGrids, float InUpdateInterval) : UpdateInterval(InUpdateInterval), TotalGrids(InTotalGrids)
		{
			RunHandle = MakeShared<std::atomic<uint64>, ESPMode::ThreadSafe>(0);
		}

		FTimerHandle CooldownTimer;
		const float UpdateInterval = 0.f;
		const int TotalGrids = 0;
		double StartTime = 0;
		
		bool IsCompleted() const { return CompletedGridsCount == TotalGrids; }
		bool IsOnCooldown() const { return CompletedGridsCount == TotalGrids && CooldownTimer.IsValid(); };
		
		void IncrementGridsCompleted() { CompletedGridsCount++; }
		void Reset(const UWorld* World) { CompletedGridsCount = 0; World->GetTimerManager().ClearTimer(CooldownTimer); }
		
		// acts as a cancellation token
		TSharedPtr<std::atomic<uint64>, ESPMode::ThreadSafe> RunHandle;
		void IncrementRunHandle() { RunHandle->fetch_add(1); };
		void ClearRunHandle() { RunHandle->store(0); };
		
	private:
		int CompletedGridsCount = 0;
	};

public:
	// Sets default values for this component's properties
	USpatialObservationComponent();
	
	UFUNCTION(BlueprintCallable)
	void SetSpatialObservationActive(bool bActive);
	
	const TArray<float>* GetRaindropData(int ConfigIndex) { return RaindropConfigsData[ConfigIndex].GetData(); }

#if WITH_EDITOR
	void Debug_SpawnActorInfront();
#endif
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void SetSpatialObservationActive_Internal(bool bActive);
	virtual void ResetRaindropData();
	
	FRaindropVariables GetVariables(int ConfigIndex, int GridIndex) const;
	
	TMap<int, FRaindropData> RaindropConfigsData;
	TMap<int, FRaindropCategorySchedule> RaindropSchedules;
	
	FCollisionQueryParams CollisionQueryParams;
	TWeakObjectPtr<const URaindropSettings> Settings;

#if WITH_EDITOR
	void ProcessRaindropDebug(const FRaindropGridDebugData& DebugData, const TArrayView<const float>& RaindropData, int ConfigIndex, int GridIndex) const;
#endif
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DebugDrawDuration = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, bool> DebugOptions;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> StressTest_SpawnActorClass;	
#endif
	
	bool bSpatialObservationActive = false;
	
private:
	FTimerHandle ResetRaindropBuffersTimer;
};

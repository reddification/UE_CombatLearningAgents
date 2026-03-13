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
		
		FRaindropGridDebugData(const FRaindropParams& RaindropParams, double InRequestedAt, int CellSpan)
		{
			RequestedAt = InRequestedAt;
			StartTime = FPlatformTime::Seconds();
			TraceMode = RaindropParams.TraceMode;
			Rows = RaindropParams.Rows;
			Columns = RaindropParams.Columns;
			if (CellSpan == 1)
			{
				Traces.Reserve(Rows * Columns);
			}
			else
			{
				int TraceRows = Rows / CellSpan + Rows % CellSpan == 0 ? 0 : 1;
				int TraceColumns = Columns / CellSpan + Columns % CellSpan == 0 ? 0 : 1;
				Traces.Reserve(TraceRows * TraceColumns);
			}
		};
	
		void Reset()
		{
			Traces.Reset();
		};

		TArray<FRaindropDebugData_Trace> Traces;
		double RequestedAt = 0;
		double StartTime = 0;
		double FinishedAt = 0;
		int Rows = 0;
		int Columns = 0;
		ERaindropTraceMode TraceMode = ERaindropTraceMode::Linetrace;
		
#pragma region boilerplate
		FRaindropGridDebugData(const FRaindropGridDebugData& Other)
			: Traces(Other.Traces),
			  RequestedAt(Other.RequestedAt),
			  StartTime(Other.StartTime),
			  FinishedAt(Other.FinishedAt),
			  Rows(Other.Rows),
			  Columns(Other.Columns),
			  TraceMode(Other.TraceMode)
		{
		}

		FRaindropGridDebugData(FRaindropGridDebugData&& Other) noexcept
			: Traces(std::move(Other.Traces)),
			  RequestedAt(Other.RequestedAt),
			  StartTime(Other.StartTime),
			  FinishedAt(Other.FinishedAt),
			  Rows(Other.Rows),
			  Columns(Other.Columns),
			  TraceMode(Other.TraceMode)
		{
		}

		FRaindropGridDebugData& operator=(const FRaindropGridDebugData& Other)
		{
			if (this == &Other)
				return *this;
			Traces = Other.Traces;
			RequestedAt = Other.RequestedAt;
			StartTime = Other.StartTime;
			FinishedAt = Other.FinishedAt;
			Rows = Other.Rows;
			Columns = Other.Columns;
			TraceMode = Other.TraceMode;
			return *this;
		}

		FRaindropGridDebugData& operator=(FRaindropGridDebugData&& Other) noexcept
		{
			if (this == &Other)
				return *this;
			Traces = std::move(Other.Traces);
			RequestedAt = Other.RequestedAt;
			StartTime = Other.StartTime;
			FinishedAt = Other.FinishedAt;
			Rows = Other.Rows;
			Columns = Other.Columns;
			TraceMode = Other.TraceMode;
			return *this;
		}
		
#pragma endregion
	};
	
#endif

public:
	// Sets default values for this component's properties
	USpatialObservationComponent();
	
	UFUNCTION(BlueprintCallable)
	void SetSpatialObservationActive(bool bActive);
	
	const TArray<float>* GetRaindropData(int ConfigIndex) { return RaindropData[ConfigIndex].GetData(); }

#if WITH_EDITOR
	void Debug_SpawnActorInfront();
#endif
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void SetSpatialObservationActive_Internal(bool bActive);
	virtual void ResetRaindropData();
	
	FRaindropVariables GetVariables(int ConfigIndex, int GridIndex) const;
	
	TMap<int, FRaindropData> RaindropData;
	
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

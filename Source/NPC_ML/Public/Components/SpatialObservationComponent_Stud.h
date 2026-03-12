// 

#pragma once

#include <chrono>

#include "CoreMinimal.h"
#include "SpatialObservationComponent.h"
#include "SpatialObservationComponent_Stud.generated.h"


UENUM()
enum class ERaindropAsyncProcessMode : uint8
{
	// do all line traces/shape sweeps first, then apply the whole grid result to raindrop buffer 
	Atomic,
	// do line traces/shape sweeps row over row in 2 passes.
	// e.g. first row 0, then 2, then 4 ... then N-1 -> then apply half of grid to raindrop buffer. 
	// Then row 1, then row 3, then row 5 ... then N -> then apply the other half of grid to raindrop buffer. 
	// Basically, you will results twice as fast but half cooked
	Stripes,
};

/*
 * I can do line traces in Async(EAsyncExecution::ThreadPool) first of all, because her father introduced me to Async(EAsyncExecution::ThreadPool)
 * And second of all, because I am a stud. I'm ballsy. I don't take no shit from anyone, I pause game thread in EndPlay until background thread finishes anywhere I want.
 * I don't have to GetWorld()->QueryTraceData() with FTraceHandles from GetWorld()->AsyncLineTraceByChannel() on the next frame. Like you. WHAHAHAHAHAH
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NPC_ML_API USpatialObservationComponent_Stud : public USpatialObservationComponent
{
	GENERATED_BODY()

private:
	using FRaindropGridKey = TPair<int, int>;
	using FRunHandle = TSharedPtr<std::atomic<uint64>, ESPMode::ThreadSafe>;
	
public:
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly))
	void Debug_DoOnce();
	
#if WITH_EDITOR	
	void Debug_CancelAsyncRaindrops();
#endif
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void SetSpatialObservationActive_Internal(bool bActive) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ERaindropAsyncProcessMode AsyncProcessMode = ERaindropAsyncProcessMode::Atomic;
	
private:
	bool IsAsyncRaindropActive() const;
	void LidarRaindropAsync_Atomic(FRaindropVariables RaindropVariables, FRaindropParams RaindropParams, int ConfigIndex, int GridIndex, const FRunHandle& RunHandle);
	void LidarRaindropAsync_Stripes(FRaindropVariables RaindropVariables, FRaindropParams RaindropParams, int ConfigIndex, int GridIndex, const FRunHandle& RunHandle);
	static void RaindropToArray(TWeakObjectPtr<UWorld> WorldWeak, const FRaindropVariables& Variables,
	                            const FRaindropParams& Params, const FCollisionQueryParams& CollisionQueryParams,
	                            TArray<float>& Array, int RowIndex, const TFunctionRef<bool()>& CanTraceFunc,
#if WITH_EDITOR
	                            FRaindropGridDebugData& DebugData, TMap<FName, bool>& DebugOptions
#endif
	                            );

	void StartAsyncRaindrop(int ConfigIndex);
	void StartAsyncRaindrop(FRaindropCategorySchedule& RaindropSchedule, int ConfigIndex);
	void CancelAsyncRaindrops();
	
#if WITH_EDITOR
	void ProcessAsyncResult(const TArray<float>& RaindropBatch, int32 ConfigIndex, int32 GridIndex, const FRaindropGridDebugData& DebugData);
#else 
	void ProcessAsyncResult(const TArray<float>& RaindropBatch, int32 ConfigIndex, int32 GridIndex);
#endif
	
#if WITH_EDITOR
	static void Debug_StressTest(TMap<FName, bool>& DebugOptions, FName StressTestKey, TWeakObjectPtr<UWorld> WorldWeak,
	                             TWeakObjectPtr<USpatialObservationComponent_Stud> ThisWeak);
#endif
	
	TSharedPtr<std::atomic<uint16>, ESPMode::ThreadSafe>  AsyncRaindropsActiveCount;
};

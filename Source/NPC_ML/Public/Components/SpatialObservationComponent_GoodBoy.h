#pragma once

#include "CoreMinimal.h"
#include "SpatialObservationComponent.h"
#include "SpatialObservationComponent_GoodBoy.generated.h"

/*
 * Missis Gipiti said that it is bad to do line traces in Async(EAsyncExecution::ThreadPool) amd i must use GetWorld()->AsyncLineTraceByChannel
 * becaus GetWorld()->LineTraceSingleByChannel is unsaef to call from background threds and good boyes don't do dangeres tings its not cool
 * I must instead keep maps with hundreds of FTraceHandle to FRaindropTraceHandle and time-slice requests because of how UE's AsyncLineTrace works 
 * (ring buffer of a size of 2 which stalls game thread on the next frame until all requested async traces are completed from previous thread,
 * so I have to literally limit how much async trace requests I can do per frame. big sad.)  
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NPC_ML_API USpatialObservationComponent_GoodBoy : public USpatialObservationComponent
{
	GENERATED_BODY()
	
private:
	struct FGridExecutor
	{
		FGridExecutor(USpatialObservationComponent_GoodBoy* Owner, int ConfigIndex, int GridIndex);
		
		int RequestAsyncTraces(int TracesBudget);
		void Restart();
		FORCEINLINE bool IsCompleted() const { return bCompleted; }

#if WITH_EDITOR
		mutable FRaindropGridDebugData DebugData;
#endif
		
		private:
			const TWeakObjectPtr<USpatialObservationComponent_GoodBoy> Owner;
			FRaindropVariables Variables;
			const int ConfigIndex = 0;
			const int GridIndex = 0;
		
			int LastRow = 0;
			int LastColumn = 0;
			uint32 TraceData = 0;
			bool bCompleted = false;
	};
	
	struct FConfigExecutor
	{
		FConfigExecutor(USpatialObservationComponent_GoodBoy* Owner, int ConfigIndex);
		
		int RequestAsyncTraces(int TracesBudget);
		bool IsCompleted() const { return bCompleted; }
		void Restart();

		const FGridExecutor& operator [] (int i) const
		{
			return GridQueue[i];
		}
		
#if WITH_EDITOR
		int GetTotalTracesCount() const;
#endif

		double RequestedAt = 0;
		
	private:
		const TWeakObjectPtr<USpatialObservationComponent_GoodBoy> Owner;
		const int ConfigIndex = 0;
		int CurrentGridIndex = 0;
		FTimerHandle RestartQueueTimer;
		FTimerDelegate RestartTimerDelegate;
		TArray<FGridExecutor, TInlineAllocator<6>> GridQueue;
		bool bCompleted = false;
	};
	
	friend struct FGridExecutor;
	friend struct FConfigExecutor;
	
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly))
	void Debug_DoOnce();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetSpatialObservationActive_Internal(bool bActive) override;
	virtual void ResetRaindropData() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMIn = 1, ClampMin = 1))
	int MaxCountOfRequests = 200;

private:
	TMap<int, FConfigExecutor> Configs;
	TArray<int, TInlineAllocator<4>> ConfigsQueue;
	FTraceDelegate TraceDelegate;
	
#if WITH_EDITOR
	bool bDebug_DoOnce = false;
#endif

	void RestartQueue(int ConfigIndex);
	int GetMaxCountOfRequests() const;
	void AsyncTraceCallback(const FTraceHandle& TraceHandle, FTraceDatum& TraceDatum);
};

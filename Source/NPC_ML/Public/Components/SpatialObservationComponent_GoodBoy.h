#pragma once

#include "CoreMinimal.h"
#include "SpatialObservationComponent.h"
#include "SpatialObservationComponent_GoodBoy.generated.h"

UENUM()
enum class EConfigAsyncRequestMode : uint8
{
	Sequential,
	Parallel
};

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
	friend struct FGridState;
	
	struct FGridState
	{
		FGridState(const FRaindropVariables& InVariables, const TSharedPtr<FTraceDelegate>& InDelegate)
			: Delegate(InDelegate), Variables(InVariables) {}
		
		int RequestAsyncTraces(int TracesBudget);
		const FRaindropParams& GetParams();
		
		TSharedPtr<FTraceDelegate> Delegate;
		FRaindropVariables Variables;
		TWeakObjectPtr<USpatialObservationComponent_GoodBoy> Owner;
		
#if WITH_EDITOR
		FRaindropGridDebugData DebugData;
#endif
		
		private:
			int Row = 0;
			int Column = 0;
			bool bCompleted = false;
	};
	
	struct FConfigState
	{
		FConfigState() {  }
		
		void RequestAsyncTraces(int TracesBudget) {};
		
	private:
		TArray<FGridState> GridStates;
		int ConfigIndex = 0;
		int GridIndex = 0;
	};
	
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
	int CurrentConfigIndex = 0;
	int CurrentGridIndex = 0;
	int RowIndex = 0;
	int ColumnIndex = 0;
	FRaindropVariables Variables;

	FTraceDelegate TraceDelegate;
	
#if WITH_EDITOR
	TMap<int, TArray<FRaindropGridDebugData>> GridsDebugDataMap;
	bool bDebug_DoOnce = false;
#endif

	void RemoveCooldown(int ConfigIndex);
	void OnGridLastTraceRequested(const float DeltaTime);
	int GetMaxCountOfRequests() const;
	int GetNextConfigIndex(int StartingFrom);
	void AsyncTraceCallback(const FTraceHandle& TraceHandle, FTraceDatum& TraceDatum);
};

#pragma once

#include "RaindropDataTypes.generated.h"

UENUM()
enum class ERaindropTraceMode : uint8
{
	Linetrace = 0,
	BoxSweep,
	SphereSweep
};

USTRUCT(BlueprintType)
struct FRaindropGridDescriptor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString UserDescription;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool Debug_DumpGridToLog = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool Debug_DrawGridShapes = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector Offset = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator Rotation = FRotator::ZeroRotator;
	
	// Bigger span -> less line traces/shape sweeps. e.g. if cell span is 3, then a single trace/sweep will be made from the center of the area (that is 1 cell forward horizontally and vertically)
	// If shape sweep is used, then shape size is adjusted for the cell area to encompass it. LIDAR value for each cell of combined cell area will be the same. 
	// Currently edge cases with sweeping out of bounds are not handled, so there could be overboard sweeps for right and bottom edges because pivot is top left corner.
	// For example, start sweep at [row size - 2] column with cell span of 5 -> sweep will be made full-size with cell span of 5,
	// hence 5-2 = 3 -> an area of 3 cells are swept overboard, potentially stopping at geometry which is overboard of raindrop grid. 
	// It will not crash, but it will catch some redundant exceeding geometry for right and bottom sides of grid in some cases, so keep that in mind 
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int CellSpan = 1;
};

USTRUCT(BlueprintType)
struct FRaindropParams
{
	GENERATED_BODY()

	// length of a side of a square that will be used as a LIDAR spatial cellularization unit
	// in unreal units (cantimeters).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CellDimension = 25.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int Rows = 15;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int Columns = 15;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<ECollisionChannel> TraceChannel;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERaindropTraceMode TraceMode = ERaindropTraceMode::Linetrace;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = 0.f, ClampMin = 0.f, EditCondition="TraceMode != ERaindropTraceMode::Linetrace"))
	float SweepShapeScale = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 0.f, ClampMin = 0.f))
	float TraceDistance = 1500.f;
	
	FORCEINLINE int GetResolution() const { return Rows * Columns; }
};

struct FRaindropData
{
	FRaindropData() = default;

	int GridsCount = 0;
	int GridSize = 0;

	explicit FRaindropData(int InGridsCount, int InGridSize)
	{
		Data.SetNumZeroed(InGridsCount * InGridSize);
		GridsCount = InGridsCount;
		GridSize = InGridSize;
	}

	FRaindropData(const FRaindropData& Other)
		: GridsCount(Other.GridsCount), GridSize(Other.GridSize), Data(Other.Data)
	{
	}

	FRaindropData(FRaindropData&& Other) noexcept
		: GridsCount(Other.GridsCount), GridSize(Other.GridSize), Data(std::move(Other.Data))
	{
		Other.GridsCount = 0;
		Other.GridSize = 0;
	}

	FRaindropData& operator=(const FRaindropData& Other)
	{
		if (this == &Other)
			return *this;
		
		Data = Other.Data;
		GridSize = Other.GridSize;
		GridsCount = Other.GridsCount;
		return *this;
	}

	FRaindropData& operator=(FRaindropData&& Other) noexcept
	{
		if (this == &Other)
			return *this;
		
		Data = MoveTemp(Other.Data);
		GridSize = Other.GridSize;
		GridsCount = Other.GridsCount;
		Other.GridsCount = 0;
		Other.GridSize = 0;
		return *this;
	}
	
	const TArray<float>* GetData() const { return &Data; }
	
	TArrayView<float> GetArrayView(int GridIndex)
	{
		return MakeArrayView(Data).Slice(GridIndex * GridSize, GridSize);	
	}

	bool StoreData(int GridIndex, const TArray<float>& Array, int Offset = 0);
	
	
private:
	TArray<float> Data;
};

struct FRaindropVariables
{
	FVector OriginLocation = FVector::ZeroVector;
	FVector RightVector = FVector::ZeroVector;
	FVector UpVector = FVector::ZeroVector;
	FVector TraceDirection = FVector::ZeroVector;
	FCollisionShape SweepShape;
	FQuat SweepRotation = FQuat::Identity;
	float TraceDistance = 2000.0;
	float BaseOffsetH = 0.f;
	float BaseOffsetV = 0.f;
	float CellCenterOffset = 0.f;
	int CellSpan = 1;
};
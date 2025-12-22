#pragma once
#include "RaindropDataTypes.generated.h"

USTRUCT(BlueprintType)
struct FLidarRaindropParams
{
	GENERATED_BODY()

	FLidarRaindropParams() : Radius(1000), Density(15) { }
	FLidarRaindropParams(int InRadius, int InDensity) : Radius(InRadius), Density(InDensity) {  }
	
	// in unreal units (cantimeters). int as data type is chosen for internal calculation convenience
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int Radius = 1000;
	
	// in unreal units (cantimeters). int as data type is chosen for internal calculation convenience
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int Density = 15;

	// in unreal units (cantimeters).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TraceDistance = 2000.f;
	
	FORCEINLINE int GetResolution() const { return Radius * 2 / Density; }
};

USTRUCT(BlueprintType)
struct FRaindropRelevancyParams
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float IrrelevancyTimeDelay = 5.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DotProductThreshold = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DistanceAttenuatedDotProductThreshold = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DistanceThreshold = 800.f;
};

struct FRaindropBuffer
{
	TArray<float> RawData;
	FGuid OccupierId = {};
	
	FORCEINLINE bool IsOccupied() const { return OccupierId.IsValid(); }
	
	FRaindropBuffer() = default;
	
	explicit FRaindropBuffer(int Size)
	{
		RawData.SetNumZeroed(Size);
	}
	
	explicit FRaindropBuffer(int Size, FGuid InOccupierId)
	{
		RawData.SetNumZeroed(Size);
		OccupierId = InOccupierId;
	}
	
	FRaindropBuffer(const FRaindropBuffer&) = default;
	FRaindropBuffer& operator=(const FRaindropBuffer&) = default;

	FRaindropBuffer(FRaindropBuffer&& Other) noexcept : RawData(MoveTemp(Other.RawData)), OccupierId(Other.OccupierId)
	{
		Other.OccupierId = {};
	}

	FRaindropBuffer& operator=(FRaindropBuffer&& Other) noexcept
	{
		if (this != &Other)
		{
			RawData = MoveTemp(Other.RawData);
			OccupierId = Other.OccupierId;
			Other.OccupierId = {};
		}
		
		return *this;
	}

	~FRaindropBuffer() = default;
};

UENUM(BlueprintType)
enum class ELARaindropTarget : uint8
{
	Self = 0,
	Ally = 1,
	Enemy = 2, 
	Neutral = 3,
};

struct FRaindropRowData
{
	FRaindropRowData(int RaindropResolution, int Row, int RaindropBufferIndex, ELARaindropTarget TargetType,
		const FGuid& OccupierId)
		: RaindropResolution(RaindropResolution),
		  Row(Row),
		  RaindropBufferIndex(RaindropBufferIndex),
		  TargetType(TargetType),
		  OccupierId(OccupierId)
	{
	}

	int RaindropResolution = 0;
	int Row = 0;
	int RaindropBufferIndex = 0;
	ELARaindropTarget TargetType;
	FGuid OccupierId;
};

struct FRaindropSynchronizationUnit
{
	std::atomic<bool> bActive { false };
	FCriticalSection CriticalSection;
};

struct FLidarRaindropVariables
{
	FLidarRaindropVariables() : SynchronizationUnit(MakeShared<FRaindropSynchronizationUnit>()) { }
	
	TSharedRef<FRaindropSynchronizationUnit> SynchronizationUnit;
	FVector OriginVector = FVector::ZeroVector;
	FVector DirectionVectorX = FVector::ZeroVector;
	FVector DirectionVectorY = FVector::ZeroVector;
	FVector TraceDirection = FVector::ZeroVector;
	double TraceDistance = 2000.0;
		
#if WITH_EDITOR
	FString LogInfo;
#endif
};

struct FRaindropBufferHandle
{
	friend struct FRaindrop;
	
	int RaindropBufferIndex = -1;
	
	FORCEINLINE const FGuid& GetOccupierId() const { return OccupierId; }
	FORCEINLINE bool IsValid() const { return RaindropBufferIndex >= 0 && OccupierId.IsValid(); }
	
private:
	FGuid OccupierId = {};
};

struct FRaindrop
{
	FRaindrop()
	{
		BufferHandle.OccupierId = FGuid::NewGuid();
	}
	
	FLidarRaindropVariables Variables;	
	FRaindropBufferHandle BufferHandle;
	float LastRelevantAt = 0.f;
	bool bRelevant = false;
};

struct FLidarSelfObservationCache
{
	float AverageCeilingHeight = 500.f;
	// essentially, matrices, but since I'm going to shove them into conv2d observations as static arrays,
	// it's more convenient to store them as 1d array from the get-go
	TArray<float>* DownwardRaindrops = nullptr; 
	TArray<float>* ForwardRaindrops = nullptr;
	TArray<float>* BackwardRaindrops = nullptr;
};
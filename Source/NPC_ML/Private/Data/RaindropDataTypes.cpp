#pragma once

#include "Data/RaindropDataTypes.h"

#include "LogChannels.h"

bool FRaindropData::StoreData(int GridIndex, const TArray<float>& Array, int Offset)
{
	if (ensure(GridIndex < GridsCount) && ensure(GridIndex * GridSize + Offset + Array.Num() <= GridsCount * GridSize))
	{
		if (Offset + Array.Num() > GridSize)
		{
			UE_LOG(LogNpcMl_Raindrop, Warning, TEXT("Potentially writing to 2 raindrop grids in FRaindropData::StoreData"))
			ensure(false);
		}
		
		void* Result = FMemory::Memcpy(Data.GetData() + GridIndex * GridSize + Offset, Array.GetData(), Array.Num() * sizeof(float));
		return ensure(Result != nullptr);
	}
	
	UE_LOG(LogNpcMl_Raindrop, Warning, TEXT("Invalid parameters in FRaindropData::StoreData: index = %d, offset = %d"), GridIndex, Offset);
	return ensure(false);
}

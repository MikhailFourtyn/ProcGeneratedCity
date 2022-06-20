// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldItem3DArray.h"

#include <string>

UWorldItem3DArray::UWorldItem3DArray()
{
	DefaultValue = NewObject<UWorldArrayItem>();
	DefaultValue->TileType = ETileType::ETT_Undefined;
}

UWorldArrayItem* UWorldItem3DArray::GetElement(int32 z, int32 y, int32 x)
{
	const int32 Index = z * Bounds.Y * Bounds.X
				+ y * Bounds.X + x;
	
	if(Arr.IsValidIndex(Index))
	{
		return Arr[Index];
	}
	else
	{
		UE_LOG(LogRoadGeneration, Error,
		TEXT("UWorldItem3DArray::GetElement - INVALID INDEX! %d\n Index: iX: %d, iY: %d, iZ: %d\nBounds: bX: %d, bY: %d, bZ: %d"),
		Index, x, y, z,
		Bounds.X, Bounds.Y, Bounds.Z);
		return nullptr;
	}
}

bool UWorldItem3DArray::SetElement(int32 z, int32 y, int32 x, UWorldArrayItem* Value)
{
	const int32 Index = z * Bounds.Y * Bounds.X
				+ y * Bounds.X + x;
	
	if(Arr.IsValidIndex(Index))
	{
		UWorldArrayItem* Item = NewObject<UWorldArrayItem>();
		Item->Init(Value->TileType, Value->ChosenTileIndex,
			Value->PossibleTiles, Value->TileRotation, Value->bIsChosen);
		Arr[Index] = Item;
		return true;
	}
	else
	{
		UE_LOG(LogRoadGeneration, Error,
		TEXT("UWorldItem3DArray::SetElement - INVALID INDEX! %d\n Index: iX: %d, iY: %d, iZ: %d\nBounds: bX: %d, bY: %d, bZ: %d"),
		Index, x, y, z,
		Bounds.X, Bounds.Y, Bounds.Z);
	}

	return false;
}

void UWorldItem3DArray::Init(int32 boundZ, int32 boundY, int32 boundX)
{
	if(Arr.Num() == 0)
	{
		Bounds = FIntVector(boundX, boundY, boundZ);
	 	
		int32 arrLen = boundZ * boundY * boundX;
		Arr.Init(DefaultValue, arrLen);
	}
}

int32 UWorldItem3DArray::GetLinearIndex(int32 z, int32 y, int32 x)
{
	if(z < 0 || y < 0 || x < 0)
	{
		UE_LOG(LogGeneration, Error,
		TEXT("UWorldItem3DArray::GetLinearIndex - INVALID INPUT! %d\n X: %d, Y: %d, Z: %d\nBounds: bX: %d, bY: %d, bZ: %d"),
		x, y, z,
		Bounds.X, Bounds.Y, Bounds.Z);
		throw new std::exception("UWorldItem3DArray::GetLinearIndex - INVALID INPUT!");
	}
	
	return z * Bounds.Y * Bounds.X
			+ y * Bounds.X + x;
}

int32 UWorldItem3DArray::GetDeltaIndex(ETileCompatibilityDeltaPosition delta)
{
	// TODO: Заменить все вычисления индекса на вызов этой функции
	// Чтоб потом в одном месте все ошибки исправлять
	switch (delta)
	{
	case ETileCompatibilityDeltaPosition::ETDP_OnForward:
		return -Bounds.X;
	case ETileCompatibilityDeltaPosition::ETDP_OnBackward:
		return Bounds.X;
	case ETileCompatibilityDeltaPosition::ETDP_OnLeft:
		return -1;
	case ETileCompatibilityDeltaPosition::ETDP_OnRight:
		return +1;
	case ETileCompatibilityDeltaPosition::ETDP_OnTop:
		return Bounds.Y * Bounds.X;
	case ETileCompatibilityDeltaPosition::ETDP_OnBottom:
		return -Bounds.Y * Bounds.X;
	default:
		return 0;
	}
}

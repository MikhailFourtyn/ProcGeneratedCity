// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TileRegistry.h"
#include "UObject/NoExportTypes.h"
#include "WorldArrayItem.h"
#include "TileRegistry.h"
#include "WorldItem3DArray.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API UWorldItem3DArray : public UObject
{
	GENERATED_BODY()
public:
	UWorldItem3DArray();
	
	UPROPERTY()	
	TArray<UWorldArrayItem*> Arr;

	UPROPERTY()
	UWorldArrayItem* DefaultValue;

	// TODO: Substitute all DoSmth(z, y, x) operations with:
	// DoSmth(GetLinearIndex(z, y, x))
	// and set Arr as public for direct operations 
	
	FIntVector Bounds;

	UWorldArrayItem* GetElement(int32 z, int32 y, int32 x);

	bool SetElement(int32 z, int32 y, int32 x, UWorldArrayItem* Value);
	
	void Init(int32 boundZ, int32 boundY, int32 boundX);

	int32 GetLinearIndex(int32 z, int32 y, int32 x);

	int32 GetDeltaIndex(ETileCompatibilityDeltaPosition delta);

	FORCEINLINE int32 GetPlusX() const { return 1; }
	FORCEINLINE int32 GetMinusX() const { return -1; }
	FORCEINLINE int32 GetPlusY() const { return Bounds.X; }
	FORCEINLINE int32 GetMinusY() const { return -Bounds.X; }
	FORCEINLINE int32 GetPlusZ() const { return Bounds.Y * Bounds.X; }
	FORCEINLINE int32 GetMinusZ() const { return -(Bounds.Y * Bounds.X); }
};

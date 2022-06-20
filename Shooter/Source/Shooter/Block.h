#pragma once

#include "CoreMinimal.h"
#include "GenerationLogs.h"
#include "Block.generated.h"

USTRUCT()
struct FBlock
{
	GENERATED_BODY()

	FIntVector StartCorner;
	FIntVector EndCorner;

	FORCEINLINE int32 GetArea()
	{
		return (EndCorner.X - StartCorner.X) * (EndCorner.Y - StartCorner.Y);
	}
	FORCEINLINE FIntVector GetBounds()
	{
		return (EndCorner - StartCorner);
	}

	// Returns longer side divided by shorter side
	// Return value [1; +infinity)
	float GetAspectRatio()
	{
		int32 DeltaX = EndCorner.X - StartCorner.X;
		int32 DeltaY = EndCorner.Y - StartCorner.Y;
		if(DeltaX == 0)
		{
			UE_LOG(LogGeneration, Error, TEXT("FBlock::GetAspectRatio() - DeltaX = 0!"));
			return 0.f;
		}
		if(DeltaY == 0)
		{
			UE_LOG(LogGeneration, Error, TEXT("FBlock::GetAspectRatio() - DeltaY = 0!"));
			return 0.f;
		}
		
		float res = (DeltaX) > (DeltaY) ?
			(float)(DeltaX) / (DeltaY) :
			(float)(DeltaY) / (DeltaX);
		if(res < 0)
			res = -res;
		return res;
	}

	void SetParams(FIntVector startCorner, FIntVector endCorner)
	{
		StartCorner = startCorner;
		EndCorner = endCorner;
	}
};
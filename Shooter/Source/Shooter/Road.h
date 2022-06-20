#pragma once

#include "CoreMinimal.h"
#include "Road.generated.h"

USTRUCT()
struct FRoad
{
	GENERATED_BODY()
	// Start and end indexes in array
	FIntVector StartPoint;
	FIntVector EndPoint;
	// bool bIsWideRoad = false;
	int RoadWidth = 1;
	int RoadLength = 0;

	bool bDirectedAlongX = true;

	// bool bIsEndPointArtificiallyAdjusted = false;
	FIntVector ArtificialAdjustmentForIntersection = FIntVector(0);

	void Init(FIntVector startPoint, FIntVector endPoint, int roadWidth, bool directedAlongX = true)
	{
		StartPoint = startPoint;
		EndPoint = endPoint;
		RoadWidth = roadWidth;
		bDirectedAlongX = directedAlongX;

		UpdateRoadLength();
	}

	void UpdateRoadLength()
	{
		if(bDirectedAlongX)
		{
			RoadLength = (EndPoint - ArtificialAdjustmentForIntersection).X - StartPoint.X;
		}
		else
		{
			RoadLength = (EndPoint - ArtificialAdjustmentForIntersection).Y - StartPoint.Y;
		}
	}
};
#pragma once

#include "CoreMinimal.h"
#include "RoadBaseCoord.generated.h"

USTRUCT()
struct FRoadBaseCoord
{
	GENERATED_BODY()

	int coord;
	int RoadWidth = 1;
	// bool bDirectedAlongX = true;
};
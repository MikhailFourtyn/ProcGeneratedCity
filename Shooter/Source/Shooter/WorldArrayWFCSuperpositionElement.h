#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "WorldArrayWFCSuperpositionElement.generated.h"

USTRUCT()
struct FWorldArrayWFCSuperpositionElement
{
	GENERATED_BODY()

	UPROPERTY()
	int32 TileIndexInRegister;
	UPROPERTY()
	ETileRotation Rotation;
	UPROPERTY()
	int32 Weight;
};
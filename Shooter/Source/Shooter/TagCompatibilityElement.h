#pragma once

#include "CoreMinimal.h"
#include "TileRotation.h"
#include "TileType.h"
#include "TagCompatibilityElement.generated.h"

USTRUCT(BlueprintType)
struct FTagCompatibilityElement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileType Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileRotation Rotation;
};
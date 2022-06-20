#pragma once

#include "CoreMinimal.h"
#include "TileRotation.h"
#include "TileCompatibilityElement.generated.h"

USTRUCT(BlueprintType)
struct FTileCompatibilityElement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ATile> Tile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileRotation Rotation;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	ATile* TileInstance = nullptr;
};
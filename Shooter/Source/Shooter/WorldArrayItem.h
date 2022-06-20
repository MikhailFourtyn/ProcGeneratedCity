// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Tile.h"
#include "TileType.h"
#include "GenerationLogs.h"
#include "WorldArrayWFCSuperpositionElement.h"
#include "WorldArrayItem.generated.h"

UCLASS()
class SHOOTER_API UWorldArrayItem : public UObject
{
	GENERATED_BODY()

	
public:
	UWorldArrayItem();

	void SetParams(ETileType tileType, TArray<FWorldArrayWFCSuperpositionElement> possibleTiles);
	
	void Init(ETileType tileType, int32 chosenTileIndex,
		TArray<FWorldArrayWFCSuperpositionElement> possibleTiles,
		ETileRotation tileRotation, bool isChosen);

	void SetType(ETileType tileType);

	void ChooseSpecificTile(FWorldArrayWFCSuperpositionElement Element);
	
	UPROPERTY()
	ETileType TileType;
	UPROPERTY()
	int32 ChosenTileIndex;
	UPROPERTY()
	TArray<FWorldArrayWFCSuperpositionElement> PossibleTiles;
	UPROPERTY()
	ETileRotation TileRotation;
	UPROPERTY()
	bool bIsChosen;
};

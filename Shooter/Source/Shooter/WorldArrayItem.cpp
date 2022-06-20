// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldArrayItem.h"

UWorldArrayItem::UWorldArrayItem() :
TileType(ETileType::ETT_MAX),
ChosenTileIndex(0),
TileRotation(ETileRotation::ETR_Undefined),
bIsChosen(false)
{

}

void UWorldArrayItem::SetParams(ETileType tileType, TArray<FWorldArrayWFCSuperpositionElement> possibleTiles)
{
	TileType = tileType;
	PossibleTiles = possibleTiles;

}

void UWorldArrayItem::Init(ETileType tileType, int32 chosenTileIndex,
	TArray<FWorldArrayWFCSuperpositionElement> possibleTiles, ETileRotation tileRotation, bool isChosen)
{
	TileType = tileType;
	ChosenTileIndex = chosenTileIndex;
	PossibleTiles = possibleTiles;
	TileRotation = tileRotation;
	bIsChosen = isChosen;
}

void UWorldArrayItem::SetType(ETileType tileType)
{
	TileType = tileType;
}

void UWorldArrayItem::ChooseSpecificTile(FWorldArrayWFCSuperpositionElement Element)
{	
	PossibleTiles = TArray<FWorldArrayWFCSuperpositionElement> {Element};
	ChosenTileIndex = Element.TileIndexInRegister;
	TileRotation = Element.Rotation;
	
	bIsChosen = true;
}

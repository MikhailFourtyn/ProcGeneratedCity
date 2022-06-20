// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "WorldArrayItem.h"
#include "TileCompatibilityDeltaPosition.h"
#include "TagCompatibilityElement.h"
#include "TileRegistry.generated.h"

USTRUCT(BlueprintType)
struct FTileRegistryEl
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ATile> Tile;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	ATile* TileInstance;

	// Tiles which can be set in forward direction (+Y) of this tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTileCompatibilityElement> OnForwardCompatible;

	// Tiles which can be set in backward direction (-Y) of this tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTileCompatibilityElement> OnBackCompatible;

	// Tiles which can be set by left (+X) of this tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTileCompatibilityElement> OnLeftCompatible;

	// Tiles which can be set by right (-X) of this tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTileCompatibilityElement> OnRightCompatible;

	// Tiles which can be set on top (+Z) of this tile
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FTileCompatibilityElement> OnTopCompatible;

	// Tiles this tile can be set on top of
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FTileCompatibilityElement> OnBottomCompatible;
};

USTRUCT(BlueprintType)
struct FTagRegistryEl
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileType TileTag;

	// Tiles which can be set in forward direction (+Y) of this tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTagCompatibilityElement> OnForwardCompatible;

	// Tiles which can be set in backward direction (-Y) of this tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTagCompatibilityElement> OnBackCompatible;

	// Tiles which can be set by left (+X) of this tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTagCompatibilityElement> OnLeftCompatible;

	// Tiles which can be set by right (-X) of this tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTagCompatibilityElement> OnRightCompatible;

	// Tiles which can be set on top (+Z) of this tile
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FTagCompatibilityElement> OnTopCompatible;

	// Tiles this tile can be set on top of
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FTagCompatibilityElement> OnBottomCompatible;
};

UCLASS()
class SHOOTER_API ATileRegistry : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileRegistry();

	// Input: Position of next tile we want to check compatibility with
	// Output: Local direction in which we want to take array
	// Example:
	// Curr - Next
	// [2:ETR_Left] - [X]
	// Output: ETileCompatibilityDeltaPosition::ETDP_OnBackward
	// ETileCompatibilityDeltaPosition GetLocalCompatibilityDeltaPositionByWorldDeltaPosition(ETileRotation CurrentRotation, ETileCompatibilityDeltaPosition WorldDeltaPosition);

	void GetArrayOfCompatibleTilesByRelativeDeltaPosition(TArray<FTileCompatibilityElement>& OutArray, FTileRegistryEl RegistryRow,
			ETileCompatibilityDeltaPosition RelativeDeltaPosition);
	
	void GetArrayOfCompatibleTagsByRelativeDeltaPosition(TArray<FTagCompatibilityElement>& OutArray, FTagRegistryEl RegistryRow,
			ETileCompatibilityDeltaPosition RelativeDeltaPosition);
	
	// Gets the delta position corresponding to the OnXCompatible array
	// Manages the turn of tile
	static ETileCompatibilityDeltaPosition GetRelativeDeltaPosition(ETileRotation MyRotation, ETileCompatibilityDeltaPosition WorldDeltaPosition);
	
	bool IsCompatible(int MyRegIndex, ETileRotation MyTileRotation,
			int ComparableRegIndex, ETileRotation ComparableTileRotation,
			ETileCompatibilityDeltaPosition DeltaPositionToCompare);

	ETileRotation GetRelativeRotationOfTheirTile(ETileRotation MyRotation, ETileRotation TheirRotation, ETileCompatibilityDeltaPosition TheirRelativePosition);

	bool DoesCompatibilityArrayContainThisTile(TArray<FTileCompatibilityElement> CompatibilityArray, int TileIndexInRegister, ETileRotation RelativeRotation);

	bool DoesTagCompatibilityArrayContainThisTag(TArray<FTagCompatibilityElement> CompatibilityArray, ETileType Tag, ETileRotation RelativeRotation);

	ETileCompatibilityDeltaPosition ReverseWorldDirection(ETileCompatibilityDeltaPosition direction) const;
	
	void Init();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitInstancePointersInCompatibilityArray(TArray<FTileCompatibilityElement> &Array);
	void InitInstancePointersInRegistryRow(FTileRegistryEl &Row);

	bool IsTileARoad(const ATile* Tile);
	bool IsTileARoadCrossroads(const ATile* Tile);
	bool IsTileARoadOneLine(const ATile* Tile);
	bool IsTileARoadDouble(const ATile* Tile);
	bool IsTileASidewalk(const ATile* Tile);
	bool IsTileASidewalkInner(const ATile* Tile);
	bool IsTileASidewalkBorderline(const ATile* Tile);
	bool IsTileASidewalkCorner(const ATile* Tile);
	bool IsTileABuilding(const ATile* Tile);
	bool IsTileAir(const ATile* Tile);
	bool CheckTileTypeTags(const ATile* Tile, const TArray<ETileType> &TypeTags) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<FWorldArrayWFCSuperpositionElement> GetSuperpositionArrayByTag(ETileType Tag) const;
	int GetRandomWeightedTileIndex(TArray<FWorldArrayWFCSuperpositionElement> Tiles);

	// Stores the compatibility rules for tiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Tiles)
	TArray<FTileRegistryEl> RegistryArray;

	// Stores the compatibility rules for tags of tiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Tiles)
	TArray<FTagRegistryEl> TagRegistryArray;
private:
	// Add here additional tiles without need of individual rules
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Tiles)
	TArray<TSubclassOf<ATile>> AdditionalTilesList;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = TileTags)
	TArray<ETileType> RoadTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = TileTags)
	TArray<ETileType> BuildingTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = TileTags)
	TArray<ETileType> SidewalkTags;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = TileTags)
	// TArray<ETileType> CrosswalksTags;
	
	TArray<int> RoadIndexes;
	
	TArray<FWorldArrayWFCSuperpositionElement> RoadSuperpositionArray;
	TArray<FWorldArrayWFCSuperpositionElement> RoadCrossroadsSuperpositionArray;
	TArray<FWorldArrayWFCSuperpositionElement> RoadOneLineSuperpositionArray;
	TArray<FWorldArrayWFCSuperpositionElement> RoadDoubleSuperpositionArray;
	TArray<FWorldArrayWFCSuperpositionElement> SidewalkInnerSuperpositionArray;
	TArray<FWorldArrayWFCSuperpositionElement> SidewalkBorderlineSuperpositionArray;
	TArray<FWorldArrayWFCSuperpositionElement> SidewalkCornerSuperpositionArray;
	TArray<FWorldArrayWFCSuperpositionElement> BuildingSuperpositionArray;
	TArray<FWorldArrayWFCSuperpositionElement> BuildingDoorSectionSuperpositionArray;
	TArray<FWorldArrayWFCSuperpositionElement> BuildingDoorCornerSuperpositionArray;
	TArray<FWorldArrayWFCSuperpositionElement> BuildingWindowSectionSuperpositionArray;
	TArray<FWorldArrayWFCSuperpositionElement> BuildingWindowCornerSuperpositionArray;
	TArray<FWorldArrayWFCSuperpositionElement> BuildingGreebleSuperpositionArray;
	TArray<FWorldArrayWFCSuperpositionElement> AirSuperpositionArray;
	// TArray<FWorldArrayWFCSuperpositionElement> CrosswalkSuperpositionArray;
	TArray<FWorldArrayWFCSuperpositionElement> FullSuperpositionArray;
};

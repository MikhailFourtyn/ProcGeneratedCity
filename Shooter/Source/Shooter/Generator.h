// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "WorldItem3DArray.h"
#include "GenerationLogs.h"
#include "TileRegistry.h"
#include "RoadAlignment.h"
#include "Road.h"
#include "RoadBaseCoord.h"
#include "Block.h"
#include "WFCGeneratorComponent.h"
#include "Generator.generated.h"

USTRUCT()
struct FRoadGenDebugValues
{
	GENERATED_BODY()

	int32 IntersectionsAmount = 0;
};

UCLASS()
class SHOOTER_API AGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGenerator();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* GenerationBounds;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UWFCGeneratorComponent* WFCGenerator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BuildingBlockHeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FIntVector MinTileElementSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseSecondRoadsGen = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseWFC = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bVerticallyConsistentColoursInBld = true;

	TArray<ATile*> GeneratedCity;
	
	bool Generate();
	
	FRoadGenDebugValues RoadGenDebugValues;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool ValidateInput();

	// Macro function - processes all the road map generation from start to finish
	void GenerateRoadsMap();

	void DrawRoadsMapInArray();

	// Makes bounds of world generation 3D array - FIntVector WorldArrayBounds
	void MakeWorldArrayBounds();

	// Defines the bounds of area where roads can be generated
	void SetupRoadGenerationAreaBounds();
	
	// Generates all basic road coordinates on X and Y axis.
	// Fills XRoadPointsArray and YRoadPointsArray
	void GenerateBasicRoadCoords();
	// Generates all coordinates along X or Y axis. Called from GenerateBasicRoadCoords()
	void GenerateBasicRoadCoordsAlongAxis(int StartBound, int EndBound, TArray<FRoadBaseCoord> &Array);
	// Generates one coordinate on X or Y axis. Called from GenerateBasicRoadCoordsAlongAxis()
	bool GenerateSingleBasicRoadCoord(int& LastRoadIndex, int EndBound, TArray<FRoadBaseCoord> &Array);

	void ValidateBasicRoadCoords();

	// Translates basic road points (X/YRoadPointsArray) into FRoads 
	void GenerateRoadsByCoords();

	// Sets parameters of array elements which correspond to road 
	void DrawRoadInArray(FRoad Road);

	// Sets parameters of array elements which correspond to road 
	void DrawBlockInArray(FBlock Block);

	void Draw3DItemsInArray();

	void ValidateWorldArrayBounds();

	// Fills array with empty elements
	void DrawArrayEmpty();
	// Sets one element of 3D array as empty
	void DrawArrayElementEmpty(int Z, int Y, int X);
	// Sets parameters of one element of 3D array that is a part of a road 
	void DrawArrayElementBasicRoad(int Y, int X, ETileType RoadType);
	
	int RoundUp(int numToRound, int multiple) const;
	int RoundDown(int numToRound, int multiple) const;

	void FillEmptyCityBorderArea();

	void LogDrawArray2DSlice(int32 ZLevel);

	FString GetLogSymbolByTileType(ETileType type) const;

	void SpawnWorldScene(TArray<UWorldArrayItem*> Array, FIntVector Bounds);

	void SpawnBuildingBlock(FBlock block);

	void SpawnBuildingBlockColumn(int x, int y, int buildingHeight, ETileType FloorType, ETileType WindowType,
		ETileRotation RotationTag,
		ATileRegistry* &reg);

	FTransform MakeTransformByRotationEnum(ETileRotation enumRot);

// BLOCKS DIVISION ================================
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="1", AllowPrivateAccess="true"), Category=BlockDivisionParams)
	float MaxAspectRatio = 4.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="1", AllowPrivateAccess="true"), Category=BlockDivisionParams)
	float AspectRatioLargeMultiplier = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="1", AllowPrivateAccess="true"), Category=BlockDivisionParams)
	int MinArea = 9;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="1", AllowPrivateAccess="true"), Category=BlockDivisionParams)
	float AreaLargeMultiplier = 4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="1", AllowPrivateAccess="true"), Category=BlockDivisionParams)
	float BlockSideIsTooShortMultiplier = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="0", ClampMax="100", AllowPrivateAccess="true"), Category=BlockDivisionParams)
	int HalfCutPercent = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="0", ClampMax="100", AllowPrivateAccess="true"), Category=BlockDivisionParams)
	int SwitchSideToCutAcrossChance = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="0", ClampMax="100", AllowPrivateAccess="true"), Category=BlockDivisionParams)
	int SkipSecondOffsetCutsAttemptChance = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="1",AllowPrivateAccess="true"), Category=BlockDivisionParams)
	int MaxBlockAreaToSkipDivision = 100;
	void MakeBlocks();

	void DivideBlocks();

	int AmountOfSuccessfulCuts = 0; // DEBUG
	
	// Returns true if such block can exist in world (for example, is not less that minimum)
	bool CheckValidResultingBlockRestrictions(FBlock block);
	// Returns true if this block can be attempted to be divided
	bool CheckDividableBlockRestrictions(FBlock block);

	bool CheckValidResultingBlockWithFuturePossibleDivision(int32 width, int32 length);

	// Makes roads inside the block by cutting it across one of axis with random offset
	// If made any successful cut:
	// - makes new FBlocks and enqueues it into Queue (to divide further)
	// - adds a new FRoad to Roads
	// - returns true
	// If haven't made any successful cut, returns false
	// 
	// TODO: Doublecheck logic FBlocks:
	// - blockToCut, dividableBlock in PerformOffsetCuts()
	// - block, firstBlock, secondBLock, OutBlockPair in PerformOneBlockCut()
	// TODO: REFACTORING Try to remove InnerCuts and make one block right after one successful cut
	bool PerformOffsetCuts(TQueue<FBlock>& Queue, FBlock block, bool CutAcrossX);

	bool PerformOneBlockCut(FBlock block, TArray<FBlock>& OutBlockPair, int32 CutCoord, bool CutAcrossX);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	UWorldItem3DArray* WorldArray; // [Z][X][Y]
	
	TArray<FRoad> Roads;
	TArray<FBlock> Blocks;
	TArray<FBlock> ResBlocks;
	// The height of the generated area array (in tiles, not in world / local coordinates)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="2", ClampMax="10", AllowPrivateAccess="true"))
	int DesirableCityHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="1", ClampMax="10", AllowPrivateAccess="true"))
	int CityHeightRandomRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="0", ClampMax="100", AllowPrivateAccess="true"))
	int WideRoadGenerationChancePercent = 20;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess="true"))
	int InnerRoadWidth = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="1", ClampMax="2", AllowPrivateAccess="true"))
	int BasicRoadWidth = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="2", ClampMax="3", AllowPrivateAccess="true"))
	int WideRoadWidth = 2;

	// Minimum large building section side (before division) + 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="5", ClampMax="100", AllowPrivateAccess="true"))
	int MinBasicRoadOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="5", ClampMax="200", AllowPrivateAccess="true"))
	int MaxBasicRoadOffset;

	// Minimum size of a resulting buildings block
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="4", ClampMax="10", AllowPrivateAccess="true"))
	int MinBlockSide;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="5", ClampMax="14", AllowPrivateAccess="true"))
	int MaxBlockSide;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="1", AllowPrivateAccess="true"))
	int MaxValidationAttempts = 5;


	// First point where a road can be set
	FIntVector StartRoadGenerationBound;
	// Last point where a road can be set
	FIntVector EndRoadGenerationBound;

	// how wide are borders around the city of roads and buildings
	int8 CityBorderWidth;

	FIntVector MinimumGeneratedArea;

	// Holds basic road points from which the roads will be later generated
	TArray<FRoadBaseCoord> XRoadPointsArray;
	TArray<FRoadBaseCoord> YRoadPointsArray;
// ==================================================================
	// DEBUG FIELDS AND METHODS!
protected:
	void CheckRoadInArrayBounds(FRoad Road) const;
private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess="true"), Category = Debug)
	FIntVector WorldArrayBounds = FIntVector(50, 50 , 2);

	int32 RoadsBeforeDivision = 0;

	TArray<FBlock> BlocksNotDivided;
	TArray<FBlock> BlocksNotAttempted;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Generator.h"
#include "WorldArrayItem.h"

#include <chrono>

#include "DrawDebugHelpers.h"
#include "ShooterGameInstance.h"
#include "ToolContextInterfaces.h"
#include "Algo/ForEach.h"

DEFINE_LOG_CATEGORY(LogRoadGeneration);
DEFINE_LOG_CATEGORY(LogGeneration);

// Sets default values
AGenerator::AGenerator() :
BuildingBlockHeight(300.f),
MinTileElementSize(FIntVector(500.f, 500.f, 300.f)),
DesirableCityHeight(2),
CityBorderWidth(0),
MinimumGeneratedArea(FIntVector(10, 10, 1)),
MinBasicRoadOffset(30),
MaxBasicRoadOffset(70),
MinBlockSide(4),
MaxBlockSide(4)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GenerationBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("GenerationBounds"));
	GenerationBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = GenerationBounds;

	WFCGenerator = CreateDefaultSubobject<UWFCGeneratorComponent>(TEXT("WFC Generator"));
}

bool AGenerator::Generate()
{
	
	MakeWorldArrayBounds();
	
	WorldArray = NewObject<UWorldItem3DArray>();
	WorldArray->Init(WorldArrayBounds.Z, WorldArrayBounds.Y, WorldArrayBounds.X);

	GenerateRoadsMap();

	RoadsBeforeDivision = Roads.Num();

	// Make FBlock array of areas between roads
	MakeBlocks();

	// Pseudo-recursively divide blocks
	DivideBlocks();
	
	// Init values of empty array
	DrawArrayEmpty();
	FillEmptyCityBorderArea();
	DrawRoadsMapInArray();

	if(bUseWFC)
	{
		// Finally, WFC
		if(WFCGenerator)
		{
			TArray<UWorldArrayItem*> WfcResult;
			bool WfcSuccess = WFCGenerator->Generate(WorldArray, WfcResult);
	
			if(WfcSuccess)
			{
				UE_LOG(LogGeneration, Display, TEXT("WFC stage 1 finished successfully!"));
				SpawnWorldScene(WfcResult, WorldArray->Bounds);
			}
			else
			{
				UE_LOG(LogGeneration, Error, TEXT("WFC stage 1 finished unsuccessfully!"));
			}

			return WfcSuccess;
		}
	}
	return false;
}

bool AGenerator::ValidateInput()
{
	TArray<FString> ErrorMsgs;
	if(MinBasicRoadOffset > MaxBasicRoadOffset)
	{
		FString msg = "INVALID INPUT! MinBasicRoadOffset must be less than MaxBasicRoadOffset!";
		ErrorMsgs.Add(msg);
	}
	if(MinBlockSide > MaxBlockSide)
	{
		FString msg = "INVALID INPUT! MinBlockSide must be equal or less than MaxBlockSide!";
		ErrorMsgs.Add(msg);
	}
	if(BasicRoadWidth > WideRoadWidth)
	{
		FString msg = "INVALID INPUT! BasicRoadWidth must be equal or less than WideRoadWidth!";
		ErrorMsgs.Add(msg);
	}

	if(ErrorMsgs.Num() == 0)
	{
		return true;
	}
	
	for(int i = 0; i < ErrorMsgs.Num(); i++)
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(9, 100.f, FColor::Black, ErrorMsgs[i]);
		}
		UE_LOG(LogGeneration, Error, TEXT("%s"), *ErrorMsgs[i]);
	}
	return false;
}

// Called when the game starts or when spawned
void AGenerator::BeginPlay()
{
	Super::BeginPlay();

	if(!ValidateInput())
	{
		return;
	}

	// If have GameInstance, get input params for city generation from it
	UShooterGameInstance* GameInstance = (Cast<UShooterGameInstance>(GetGameInstance()));
	if(GameInstance)
	{
		GameInstance->LoadGenerationParams(
			MinBlockSide, MaxBlockSide,
			MinBasicRoadOffset, MaxBasicRoadOffset,
			WideRoadGenerationChancePercent);
	}

	Generate();
}

// Called every frame
void AGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGenerator::ValidateBasicRoadCoords()
{
	// Validating basic road coords
	
	if(XRoadPointsArray.Num() < 2)
	{
		FRoadBaseCoord coord1;
		coord1.coord = 0;
		coord1.RoadWidth = 1;
		FRoadBaseCoord coord2;
		coord2.coord = WorldArrayBounds.X - 1;
		coord2.RoadWidth = 1;
		XRoadPointsArray.Add(coord1);
		XRoadPointsArray.Add(coord2);
	}
	if(YRoadPointsArray.Num() < 2)
	{
		FRoadBaseCoord coord1;
		coord1.coord = 0;
		coord1.RoadWidth = 1;
		FRoadBaseCoord coord2;
		coord2.coord = WorldArrayBounds.Y - 1;
		coord2.RoadWidth = 1;
		YRoadPointsArray.Add(coord1);
		YRoadPointsArray.Add(coord2);
	}
}

void AGenerator::GenerateRoadsMap()
{
	// Define the bounds of area where roads can be generated
	SetupRoadGenerationAreaBounds();

	// Generates basic coords along X and Y axis - determines the positions of the lines of the road grid
	GenerateBasicRoadCoords();

	ValidateBasicRoadCoords();

	// Translates basic road coords into geometric roads
	GenerateRoadsByCoords();
	check(Roads.Num() >= 1);
}


void AGenerator::MakeWorldArrayBounds()
{
	ValidateWorldArrayBounds();
}

void AGenerator::SetupRoadGenerationAreaBounds()
{
	// Choose area where roads can be generated
	// StartRoadGenerationBound = FIntVector(1, 1, 0);
	// EndRoadGenerationBound = FIntVector(WorldArrayBounds.X - 1, WorldArrayBounds.Y - 1, 0);
	StartRoadGenerationBound = FIntVector(1, 1, 0);
	EndRoadGenerationBound = FIntVector(WorldArrayBounds.X - 1, WorldArrayBounds.Y - 1, 0);
	// +2 / -2 is for border of a house and a sidewalk
	
	check(EndRoadGenerationBound.X - StartRoadGenerationBound.X >= 0
			&& EndRoadGenerationBound.Y - StartRoadGenerationBound.Y > 0);
	
	check(EndRoadGenerationBound.Y - StartRoadGenerationBound.Y >= 0
			&& EndRoadGenerationBound.X - StartRoadGenerationBound.X > 0);
}

void AGenerator::GenerateBasicRoadCoords()
{
	GenerateBasicRoadCoordsAlongAxis(StartRoadGenerationBound.X, EndRoadGenerationBound.X, XRoadPointsArray);
	GenerateBasicRoadCoordsAlongAxis(StartRoadGenerationBound.Y, EndRoadGenerationBound.Y, YRoadPointsArray);
}

void AGenerator::GenerateBasicRoadCoordsAlongAxis(int StartBound, int EndBound, TArray<FRoadBaseCoord> &Array)
{
	int LastRoadIndex = StartBound;
	bool success = true;
	
	while(success)
	{
		success = GenerateSingleBasicRoadCoord(LastRoadIndex, EndBound, Array);
		// else - break cycle
	}
}

bool AGenerator::GenerateSingleBasicRoadCoord(int& LastRoadIndex, int EndBound, TArray<FRoadBaseCoord> &Array)
{
	int resultCoord;
	FRoadBaseCoord roadCoord;
	
	if(WideRoadGenerationChancePercent == 0)
	{
		roadCoord.RoadWidth = BasicRoadWidth;
	}
	else if(WideRoadGenerationChancePercent <= 50 && Array.Num() - 1 >= 0 && Array[Array.Num()-1].RoadWidth == WideRoadWidth)
	{
		// Prevent adjacency of two wide roads
		// If there should be less than a half of wide roads
		// and if previous road is wide, this road can't be wide
		roadCoord.RoadWidth = BasicRoadWidth;
	}
	else
	{
		if(FMath::RandRange(1, 100) <= WideRoadGenerationChancePercent)
		{
			roadCoord.RoadWidth = WideRoadWidth;
		}
		else
		{
			roadCoord.RoadWidth = BasicRoadWidth;
		}
	}
	
	if(Array.Num() == 0)
	{
		resultCoord = LastRoadIndex;
	}
	else
	{
		int offset = FMath::RandRange(MinBasicRoadOffset, MaxBasicRoadOffset);
		// Min and max coordinates across the width of the road
		int currRoadMinGeneratedCoord = LastRoadIndex + offset;
		int currRoadMaxGeneratedCoord = currRoadMinGeneratedCoord + (roadCoord.RoadWidth - 1);

		// Check if either min or max coordinate across the width of the road is outside the bounding area - current road is invalid
		if(currRoadMinGeneratedCoord >= EndBound || currRoadMaxGeneratedCoord >= EndBound)
		{
			// generated road is outside the available area
			
			return false;
		}
		
		resultCoord = currRoadMinGeneratedCoord;
		
		LastRoadIndex = resultCoord;
	}
		
	if(roadCoord.RoadWidth > 1)
	{
		// If road is wide, set LastRoadIndex for next roads generation onto the last, not first, position of road
		LastRoadIndex += (WideRoadWidth - 1);
	}
	roadCoord.coord = resultCoord;
	
	Array.Add(roadCoord);
	return true;
}

void AGenerator::GenerateRoadsByCoords()
{
	check(XRoadPointsArray.Num() >= 2);
	check(YRoadPointsArray.Num() >= 2);

	// generating roads that go along Y
	for(int i = 0; i < YRoadPointsArray.Num(); i++)
	{
		for(int j = 1; j < XRoadPointsArray.Num(); j++)
		{
			FRoad road;
			road.StartPoint = FIntVector(XRoadPointsArray[j - 1].coord, YRoadPointsArray[i].coord, 0);
			road.EndPoint = FIntVector(XRoadPointsArray[j].coord, YRoadPointsArray[i].coord, 0);
			road.RoadWidth = YRoadPointsArray[i].RoadWidth;
			road.bDirectedAlongX = false;

			if(j == XRoadPointsArray.Num() - 1)
			{
				// If current road is last in current row - prolong to fix the last angle point, like here:
				/*
				 * Before: 
				LogRoadGeneration: 0 0 0 1 1 1 0
				LogRoadGeneration: 0 0 0 1 1 1 0
				LogRoadGeneration: 1 1 1 1 1 1 0
				LogRoadGeneration: 1 1 1 1 0 0 0
				LogRoadGeneration: 0 0 0 0 0 0 0
				 * After: 
				LogRoadGeneration: 0 0 0 1 1 1 0
				LogRoadGeneration: 0 0 0 1 1 1 0
				LogRoadGeneration: 1 1 1 1 1 1 0
				LogRoadGeneration: 1 1 1 1 1 1 0
				LogRoadGeneration: 0 0 0 0 0 0 0
				 */
				if(XRoadPointsArray[XRoadPointsArray.Num() - 1].RoadWidth != 1
					&& YRoadPointsArray[YRoadPointsArray.Num() - 1].RoadWidth != 1)
				{
					// we don't get a cut angle if any of two roads is 1 tile wide

					// prolong the road by (width_of_intersecting_road - 1), because two roads already have 1 tile of intersection - the EndPoint
					road.ArtificialAdjustmentForIntersection = FIntVector(XRoadPointsArray[XRoadPointsArray.Num() - 1].RoadWidth - 1, 0, 0);
					road.EndPoint += road.ArtificialAdjustmentForIntersection;
				}
			}
			
			Roads.Add(road);
		}
	}

	// generating roads that go along X
	for(int i = 0; i < XRoadPointsArray.Num(); i++)
	{
		for(int j = 1; j < YRoadPointsArray.Num(); j++)
		{
			FRoad road;
			road.StartPoint = FIntVector(XRoadPointsArray[i].coord, YRoadPointsArray[j - 1].coord, 0);
			road.EndPoint = FIntVector(XRoadPointsArray[i].coord, YRoadPointsArray[j].coord, 0);
			road.RoadWidth = XRoadPointsArray[i].RoadWidth;
			road.bDirectedAlongX = true;
			Roads.Add(road);

			// we already prolonged the end points of roads that go along Y, no need to prolong roads that go along X here
		}
	}
}


void AGenerator::DrawRoadsMapInArray()
{
	// Draw all roads into array
	RoadGenDebugValues.IntersectionsAmount = 0;
	// UE_LOG(LogGeneration, Warning, TEXT("AGenerator::DrawRoadsMapInArray(): "));
	
	for(int x = 0; x < XRoadPointsArray.Num(); x++)
		for(int y = 0; y < YRoadPointsArray.Num(); y++)
		{
			FIntVector currPoint = FIntVector(XRoadPointsArray[x].coord, YRoadPointsArray[y].coord, 0);
			FIntVector endPoint = currPoint + FIntVector(XRoadPointsArray[x].RoadWidth - 1, YRoadPointsArray[y].RoadWidth - 1, 0);

			for(int xx = currPoint.X; xx <= endPoint.X; xx++)
			{
				for(int yy = currPoint.Y; yy <= endPoint.Y; yy++)
				{
					DrawArrayElementBasicRoad(yy, xx, ETileType::ETT_Road_Crossroads);
				}
			}
		}
	
	for(int i = 0; i < Roads.Num(); i++)
	{
		DrawRoadInArray(Roads[i]);
	}
	// UE_LOG(LogGeneration, Warning, TEXT("AGenerator::DrawRoadsMapInArray(): END"));

	for(int i = 0; i < Blocks.Num(); i++)
	{
		DrawBlockInArray(Blocks[i]);
	}

	Draw3DItemsInArray();

	for(int z = 0; z < WorldArray->Bounds.Z; z++)
	{
		LogDrawArray2DSlice(z);
	}
}

void AGenerator::DrawRoadInArray(FRoad Road)
{
	CheckRoadInArrayBounds(Road);

	ETileType BaseRoadType = ETileType::ETT_Road;
		
	if(Road.bDirectedAlongX)
	{
		for(int i = Road.StartPoint.Y; i <= Road.EndPoint.Y; i++)
		{
			{
				for(int j = 0; j < Road.RoadWidth; j++)
				{
					ETileType currentType = BaseRoadType;
					DrawArrayElementBasicRoad(i, Road.StartPoint.X + j, currentType);
				}
			}
		}
	}
	else
	{
		// Road directed along Y
		for(int i = Road.StartPoint.X; i <= Road.EndPoint.X; i++)
		{
			for(int j = 0; j < Road.RoadWidth; j++)
			{
				ETileType currentType = BaseRoadType;
				DrawArrayElementBasicRoad(Road.StartPoint.Y + j, i, currentType);
			}
		}
	}
}


void AGenerator::DrawArrayElementBasicRoad(int Y, int X, ETileType RoadType)
{
	if(Y > WorldArrayBounds.Y - 1
	|| X > WorldArrayBounds.X - 1
	|| Y < 0 || X < 0)
	{
		UE_LOG(LogRoadGeneration, Error, TEXT("AGenerator::DrawArrayElementBasicRoad ERROR: Invalid input coord! /nInput: X:%d, Y:%d;/nWorld: X:%d, Y:%d."),
			X, Y, WorldArrayBounds.X, WorldArrayBounds.Y);

		return;
	}

	UWorldArrayItem* Item = WorldArray->GetElement(0, Y, X);
	if(!Item)
	{
		Item = NewObject<UWorldArrayItem>();
	}

	if(Item->TileType != ETileType::ETT_Road_Crossroads)
	{
		Item->SetType(RoadType);
	}
	
	WorldArray->SetElement(0, Y, X, Item);
}

void AGenerator::DrawBlockInArray(FBlock Block)
{
	for(int x = Block.StartCorner.X; x <= Block.EndCorner.X; x++)
	{
		for(int y = Block.StartCorner.Y; y <= Block.EndCorner.Y; y++)
		{
			UWorldArrayItem* Item = NewObject<UWorldArrayItem>();
			if(x == Block.StartCorner.X && y == Block.StartCorner.Y
				|| x == Block.StartCorner.X && y == Block.EndCorner.Y
				|| x == Block.EndCorner.X && y == Block.StartCorner.Y
				|| x == Block.EndCorner.X && y == Block.EndCorner.Y
			)
			{
				Item->SetType(ETileType::ETT_Sidewalks_Corner);
			}
			else if(x == Block.StartCorner.X || x == Block.EndCorner.X || y == Block.StartCorner.Y || y == Block.EndCorner.Y)
			{
				Item->SetType(ETileType::ETT_Sidewalks_Borderline);
			}
			else
			{
				Item->SetType(ETileType::ETT_Sidewalks_Inner);
			}
			// Superposition array is set after the full map generation
			WorldArray->SetElement(0, y, x, Item);
		}
	}
			
}

void AGenerator::Draw3DItemsInArray()
{
	FIntVector start, end;
	
	start.X = XRoadPointsArray[0].coord;
	start.Y = YRoadPointsArray[0].coord;
	start.Z = 1;
	end.X = XRoadPointsArray[XRoadPointsArray.Num()-1].coord + XRoadPointsArray[XRoadPointsArray.Num()-1].RoadWidth - 1;
	end.Y = YRoadPointsArray[YRoadPointsArray.Num()-1].coord + YRoadPointsArray[YRoadPointsArray.Num()-1].RoadWidth - 1;
	end.Z = WorldArrayBounds.Z - 1;
	
	// Setup everything as Air
	for(int z = start.Z; z <= end.Z; z++)
	{
		for(int y = start.Y; y <= end.Y; y++)
		{	
			for(int x = start.X; x <= end.X; x++)
			{
				UWorldArrayItem* Air = NewObject<UWorldArrayItem>();
				Air->SetType(ETileType::ETT_Air);
			
				WorldArray->SetElement(z, y, x, Air);
			}
		}
	}

	// Setup inner area of each block as Building
	for(int b = 0; b < Blocks.Num(); b++)
	{
		for(int z = 1; z < WorldArray->Bounds.Z; z++)
		{
			for(int y = Blocks[b].StartCorner.Y+1; y <= Blocks[b].EndCorner.Y-1; y++)
			{	
				for(int x = Blocks[b].StartCorner.X+1; x <= Blocks[b].EndCorner.X-1; x++)
				{
					ETileType currType = ETileType::ETT_Building;
					if(x == Blocks[b].StartCorner.X+1 && y == Blocks[b].StartCorner.Y+1
						|| x == Blocks[b].EndCorner.X-1 && y == Blocks[b].StartCorner.Y+1
						|| x == Blocks[b].StartCorner.X+1 && y == Blocks[b].EndCorner.Y-1
						|| x == Blocks[b].EndCorner.X-1 && y == Blocks[b].EndCorner.Y-1
						)
					{
						// current is corner of building block
						
						if(z == 1)
							currType = ETileType::ETT_Building_Door_Corner;
						else
							currType = ETileType::ETT_Building_Window_Corner;
					}
					else
					{
						// not any corner => check for border
						if(x == Blocks[b].StartCorner.X+1
							|| x == Blocks[b].EndCorner.X-1
							|| y == Blocks[b].StartCorner.Y+1
							|| y == Blocks[b].EndCorner.Y-1
							)
						{
							if(z == 1)
								currType = ETileType::ETT_Building_Door_Section;
							else
								currType = ETileType::ETT_Building_Window_Section;
						}
						else
						{
							// not corner, not border => inside of block
							currType = ETileType::ETT_Building_Greeble_Cube;
						}
					}
					
					UWorldArrayItem* Building = NewObject<UWorldArrayItem>();
					Building->SetType(currType);
			
					WorldArray->SetElement(z, y, x, Building);
				}
			}
		}
	}
}

void AGenerator::DrawArrayElementEmpty(int Z, int Y, int X)
{
	if(Y > WorldArrayBounds.Y - 1
	|| X > WorldArrayBounds.X - 1
	|| Y < 0 || X < 0)
	{
		UE_LOG(LogRoadGeneration, Error, TEXT("AGenerator::DrawArrayElementEmpty ERROR: Invalid input coord! /nInput: X:%d, Y:%d;/nWorld: X:%d, Y:%d."),
			X, Y, WorldArrayBounds.X, WorldArrayBounds.Y);

		return;
	}
	
	UWorldArrayItem* Item = NewObject<UWorldArrayItem>();
	// Item->SetParams(ETileType::ETT_Undefined, TArray<FWorldArrayWFCSuperpositionElement> {});
	Item->SetType(ETileType::ETT_Undefined);
			
	WorldArray->SetElement(Z, Y, X, Item);
}

void AGenerator::DrawArrayEmpty()
{
	for(int z = 0; z < WorldArrayBounds.Z; z++)
	{
		for(int y = 0; y < WorldArrayBounds.Y; y++)
		{	
			for(int x = 0; x < WorldArrayBounds.X; x++)
			{
				DrawArrayElementEmpty(z, y, x);
			}
		}
	}
}


int AGenerator::RoundUp(int numToRound, int multiple) const
{
	if (multiple == 0)
		return numToRound; 

	int remainder = abs(numToRound) % multiple;
	if (remainder == 0)
		return numToRound;
	if (numToRound < 0)
		return -(abs(numToRound) - remainder);
	return numToRound + multiple - remainder;
}

int AGenerator::RoundDown(int numToRound, int multiple) const
{
	if (multiple == 0)
		return numToRound; 

	int remainder = abs(numToRound) % multiple;
	if (remainder == 0)
		return numToRound;
	if (numToRound < 0)
		return -(abs(numToRound) + multiple - remainder);
	return numToRound - remainder;
}

void AGenerator::ValidateWorldArrayBounds()
{
	if(WorldArrayBounds.X < MinimumGeneratedArea.X)
	{
		WorldArrayBounds.X = MinimumGeneratedArea.X;
	}
	if(WorldArrayBounds.Y < MinimumGeneratedArea.Y)
	{
		WorldArrayBounds.Y = MinimumGeneratedArea.Y;
	}
	if(WorldArrayBounds.Z < MinimumGeneratedArea.Z)
	{
		WorldArrayBounds.Z = MinimumGeneratedArea.Z;
	}
}

void AGenerator::CheckRoadInArrayBounds(FRoad Road) const
{
	check(Road.StartPoint.X >= 0);
	check(Road.StartPoint.Y >= 0);
	check(Road.EndPoint.X >= 0);
	check(Road.EndPoint.Y >= 0);
	check(Road.StartPoint.X <= WorldArrayBounds.X);
	check(Road.StartPoint.Y <= WorldArrayBounds.Y);
	check(Road.EndPoint.X <= WorldArrayBounds.X);
	check(Road.EndPoint.Y <= WorldArrayBounds.Y);

	// checking for wide roads
	check(Road.bDirectedAlongX ? Road.StartPoint.Y : Road.StartPoint.X
			+ Road.RoadWidth - 1 >= 0)
}

FString AGenerator::GetLogSymbolByTileType(ETileType type) const
{
	// switch (type)
	// {
	// case ETileType::ETT_Road:
	// 	return FString::Printf(TEXT("+"));
	// case ETileType::ETT_Sidewalks:
	// 	return FString::Printf(TEXT("_"));
	// default:
	// 	return FString::Printf(TEXT(" "));
	// }
	
	// switch (type)
	// {
	// case ETileType::ETT_Road:
	// case ETileType::ETT_Road_Crossroads:
	// case ETileType::ETT_Road_OneLine:
	// case ETileType::ETT_Road_HalfOfWideRoad:
	// 	return FString::Printf(TEXT("+"));
	// case ETileType::ETT_Sidewalks_Borderline:
	// case ETileType::ETT_Sidewalks_Inner:
	// case ETileType::ETT_Sidewalks_Corner:
	// 	return FString::Printf(TEXT("B"));
	// case ETileType::ETT_NoCity:
	// default:
	// 	return FString::Printf(TEXT(" "));
	// }
	
	switch (type)
		{
		case ETileType::ETT_Road:
			return FString::Printf(TEXT("r"));
		case ETileType::ETT_Road_Crossroads:
			return FString::Printf(TEXT("+"));
		case ETileType::ETT_Road_OneLine:
			return FString::Printf(TEXT("1"));
		case ETileType::ETT_Road_HalfOfWideRoad:
			return FString::Printf(TEXT("2"));
		case ETileType::ETT_Sidewalks_Borderline:
			return FString::Printf(TEXT("B"));
		case ETileType::ETT_Sidewalks_Inner:
			return FString::Printf(TEXT("I"));
		case ETileType::ETT_Sidewalks_Corner:
			return FString::Printf(TEXT("C"));
		case ETileType::ETT_NoCity:
			return FString::Printf(TEXT("~"));

		case ETileType::ETT_Building:
			return FString::Printf(TEXT("b"));
		case ETileType::ETT_Building_Door_Section:
			return FString::Printf(TEXT("s"));
		case ETileType::ETT_Building_Window_Section:
			return FString::Printf(TEXT("S"));
		case ETileType::ETT_Building_Door_Corner:
			return FString::Printf(TEXT("c"));
		case ETileType::ETT_Building_Window_Corner:
			return FString::Printf(TEXT("C"));
		case ETileType::ETT_Building_Greeble_Cube:
			return FString::Printf(TEXT("G"));
		case ETileType::ETT_Air:
			return FString::Printf(TEXT("a"));
		
		default:
			return FString::Printf(TEXT("."));
		}
}

void AGenerator::LogDrawArray2DSlice(int32 ZLevel)
{
	TArray<FString> lines;

	// lines.Add(FString::Printf(TEXT("Roads: %d"), Roads.Num()));
	// for(int i = 0; i < Roads.Num(); i++)
	// {
	// 	lines.Add(FString::Printf(TEXT("i: %d; S: %d %d; E: %d %d; Along %c"),
	// 		i,
	// 		Roads[i].StartPoint.X, Roads[i].StartPoint.Y,
	// 		Roads[i].EndPoint.X, Roads[i].EndPoint.Y,
	// 		Roads[i].bDirectedAlongX ? 'X' : 'Y'
	// 		));
	// }
	
	lines.Add(FString(TEXT("Array:")));
	lines.Add(FString::Printf(TEXT("Vert: Y, Horiz: X, Z: %d"), ZLevel));
	
	for(int y = 0; y < WorldArrayBounds.Y; y++)
	{
		FString curr = "";

		// Show Y coordinate
		if(y <= 9)
			curr += FString::Printf(TEXT("Y=%d    "), y);
		else if(y <= 99)
			curr += FString::Printf(TEXT("Y=%d   "), y);
		else
			curr += FString::Printf(TEXT("Y=%d  "), y);			
		
		for(int x = 0; x < WorldArrayBounds.X; x++)
		{
			ETileType Element = WorldArray->GetElement(ZLevel, y, x)->TileType;
			FString addedString;
			addedString = GetLogSymbolByTileType(Element);
			
			curr += addedString;
		}
		lines.Add(curr);
	}	

	// The top line shown in log is the oldest one
	for(int i = 0; i < lines.Num(); i++)
	{
		UE_LOG(LogRoadGeneration, Log, TEXT("%s"), *lines[i]);
	}
}

void AGenerator::SpawnWorldScene(TArray<UWorldArrayItem*> Array, FIntVector Bounds)
{
	ATileRegistry* reg = WFCGenerator->GetTileRegistryActor();
	for(int z = 0; z < Bounds.Z; z++)
	{
		for(int y = 0; y < Bounds.Y; y++)
		{
			for(int x = 0; x < Bounds.X; x++)
			{
				int32 LinearIndex = z * Bounds.Y * Bounds.X + y * Bounds.X + x;
				if(Array[LinearIndex]->bIsChosen
					&& Array[LinearIndex]->TileType != ETileType::ETT_Air
					&& Array[LinearIndex]->TileType != ETileType::ETT_NoCity)
				{
					if(Array[LinearIndex]->PossibleTiles.Num() > 1)
					{
						UE_LOG(LogGeneration, Error, TEXT("AGenerator::SpawnWorldScene - Array[LinearIndex]->PossibleTiles.Num() > 1!"));
					}
					
					if(Array[LinearIndex]->PossibleTiles.Num() == 0)
					{
						UE_LOG(LogGeneration, Error, TEXT("AGenerator::SpawnWorldScene - Array[LinearIndex]->PossibleTiles.Num() == 0!"));
						continue;
					}
					
					FVector BaseLocation = GetActorLocation()
						+ FVector(x * MinTileElementSize.X, y * MinTileElementSize.Y, z * MinTileElementSize.Z);

					FRotator BaseRotation = FRotator::ZeroRotator;

					FTransform BaseTransform = FTransform(BaseRotation, BaseLocation);
				
					// Make rotation by array el rotation
					FTransform MeshInnerTransform = MakeTransformByRotationEnum(Array[LinearIndex]->TileRotation);

					FVector ResultingLocation = BaseLocation + MeshInnerTransform.GetLocation();
					FQuat ResultingQuatRotation = MeshInnerTransform.GetRotation();
					FRotator ResultingRotation = ResultingQuatRotation.Rotator();
					FTransform ResultingTransform = FTransform(ResultingQuatRotation, ResultingLocation);

					// Spawn
					int32 currTileRegIndex = Array[LinearIndex]->ChosenTileIndex;
					TSubclassOf<ATile> currTileClass = reg->RegistryArray[currTileRegIndex].Tile;
					ATile* newTile = GetWorld()->SpawnActor<ATile>(currTileClass, ResultingLocation, ResultingRotation);
					GeneratedCity.Add(newTile);
				}
			}
		}
	}
}

FTransform AGenerator::MakeTransformByRotationEnum(ETileRotation enumRot)
{
	FRotator Rotation;
	FVector Location;
	switch (enumRot)
	{
	case ETileRotation::ETR_Undefined:
		UE_LOG(LogGeneration, Error, TEXT("AGenerator::MakeTransformByRotationEnum - ETileRotation::ETR_Undefined"));
	case ETileRotation::ETR_Forward:
		Location = FVector(0);
		Rotation = FRotator(0);
		break;
	case ETileRotation::ETR_Backward:
		Location = FVector(MinTileElementSize.X, -MinTileElementSize.Y, 0);
		Rotation = FRotator(0, 180, 0);
		break;
	case ETileRotation::ETR_Left:
		Location = FVector(MinTileElementSize.X, 0, 0);
		Rotation = FRotator(0, -90, 0);
		break;
	case ETileRotation::ETR_Right:
		Location = FVector(0, -MinTileElementSize.Y, 0);
		Rotation = FRotator(0, 90, 0);
		break;
	}

	FTransform res = FTransform();
	res.SetScale3D(FVector(1.f));
	res.SetLocation(Location);
	res.SetRotation(Rotation.Quaternion());

	return res;
}

void AGenerator::MakeBlocks()
{
	for(int y = 1; y < YRoadPointsArray.Num(); y++)
	{
		for(int x = 1; x < XRoadPointsArray.Num(); x++)
		{
			FBlock block;

			block.StartCorner = FIntVector(
				XRoadPointsArray[x-1].coord + XRoadPointsArray[x-1].RoadWidth,
				YRoadPointsArray[y-1].coord + YRoadPointsArray[y-1].RoadWidth,
				0);
			block.EndCorner = FIntVector(
				XRoadPointsArray[x].coord - 1,
				YRoadPointsArray[y].coord - 1,
				0);
								
			Blocks.Add(block);
		}
	}
}

void AGenerator::DivideBlocks()
{
	TQueue<FBlock> Queue;
	ResBlocks.Empty();

	AmountOfSuccessfulCuts = 0;
	
	for(int i = 0; i < Blocks.Num(); i++)
	{
		Queue.Enqueue(Blocks[i]);
	}

	int DebugQueueIsNotEmpty = 0;
	while(!Queue.IsEmpty())
	{
		DebugQueueIsNotEmpty++;

		if(DebugQueueIsNotEmpty >= 10000)
		{
			int a = 27;
			a++;
			UE_LOG(LogRoadGeneration, Error, TEXT("DivideBlocks - infinite cycle!!!"));
			break;
		}

		FBlock block;
		if(Queue.Dequeue(block))
		{
			// Check if this block should be attempted to be divided at all
			if(CheckDividableBlockRestrictions(block))
			{
				const bool LongerSideIsX = block.GetBounds().X > block.GetBounds().Y;

				// true => we go from min X to max X to make cuts
				bool CutAcrossX = !LongerSideIsX;

				// Switch cut side randomly
				if(FMath::RandRange(1, 100) <= SwitchSideToCutAcrossChance)
				{
					CutAcrossX = !CutAcrossX;
				}

				bool MadeInnerCut = PerformOffsetCuts(Queue, block, CutAcrossX);

				if(!MadeInnerCut)
				{
					// if haven't made any cuts, try another side with a chance to skip
					// don't allow skipping if the block is too large
					if(!(FMath::RandRange(1, 100) <= SkipSecondOffsetCutsAttemptChance
							&& block.GetArea() > MaxBlockAreaToSkipDivision)
							)
					{
						MadeInnerCut = PerformOffsetCuts(Queue, block, !CutAcrossX);
					}
				}
				
				if(MadeInnerCut)
				{
					// successful cut at either side
					// blocks have been already made and added to Queue, road was already made
					continue;
				}
				else
				{
					// if can't make a cut on either side
					// put block into result array

					FBlock notDivided;
					notDivided.StartCorner = block.StartCorner;
					notDivided.EndCorner = block.EndCorner;
					BlocksNotDivided.Add(notDivided);
										
					ResBlocks.Add(block);
				}
			}
			else
			{
				// if couldn't even attempt to cut this block, put this block out of the queue, into the resulting blocks array
				ResBlocks.Add(block);

				FBlock notAttempted;
				notAttempted.StartCorner = block.StartCorner;
				notAttempted.EndCorner = block.EndCorner;
				BlocksNotAttempted.Add(notAttempted);
			}
		}
		else
		{
			// can't dequeue
			break;
		}
	}
	if(ResBlocks.Num() != Blocks.Num() + AmountOfSuccessfulCuts)
	{
		UE_LOG(LogGeneration, Error,
			TEXT("ResBlocks.Num(), Blocks.Num(), AmountOfSuccessfulCuts: %d != %d + %d => false"), ResBlocks.Num(), Blocks.Num(), AmountOfSuccessfulCuts);
		
	}
	
	Blocks = ResBlocks;
}

bool AGenerator::PerformOffsetCuts(TQueue<FBlock>& Queue, FBlock blockToCut, bool CutAcrossX)
{
	TArray<int32> InnerRoadCuts;
	bool MadeInnerCut = false;
	// i - moving coordinate of a thin inner road
	// we calculate offset for roads => i holds a coordinate of road - not of a block corner
	int32 start = CutAcrossX ? blockToCut.StartCorner.X - 1 : blockToCut.StartCorner.Y - 1;
	int32 currRoad = start;
	int32 Bound = CutAcrossX ? blockToCut.EndCorner.X : blockToCut.EndCorner.Y;
	int32 BlockWidth = CutAcrossX ? blockToCut.GetBounds().Y : blockToCut.GetBounds().X;

	while(currRoad < Bound)
	{

		// offset for the next road
		int32 Offset = FMath::RandRange(MinBlockSide + 1, MaxBlockSide + 1);
		int32 NextRoad = currRoad + Offset;
		if(NextRoad < Bound)
		{
			// current cutting road is inside the block
			
			int32 NextBlockEnd = NextRoad + MinBlockSide;
			if(NextBlockEnd <= Bound)
			{
				// the next potential building block is not less than minimum

				
				int32 CuttingBlockWidth = BlockWidth;
				int32 CuttingBlockLength = (InnerRoadCuts.Num() == 0 ? NextRoad - start - 1 : NextRoad - InnerRoadCuts[InnerRoadCuts.Num()-1] - 1);
				bool CuttingBlockValid = CheckValidResultingBlockWithFuturePossibleDivision(CuttingBlockWidth, CuttingBlockLength);
				
				if(CuttingBlockValid)
				{
					InnerRoadCuts.Add(NextRoad);
					MadeInnerCut = true;
					// i += Offset;
					currRoad = NextRoad;
				}
				else
				{
					// i += Offset;
					currRoad = NextRoad;
				}
				
				if(NextBlockEnd == Bound)
				{
					// the next potential building block is precisely minimum wide - we can't cut it further, break cycle
					break;
				}
			}
			else
			{
				// the next potential building block is LESS than minimum.
				// current cutting road is outside the block
				
				// if it's our first attempt to cut => with rand chance, try cutting in half
				if(currRoad == start)
				{
					if(FMath::RandRange(1, 100) <= HalfCutPercent)
					{
						int Width = CutAcrossX ? blockToCut.GetBounds().X : blockToCut.GetBounds().Y;
						int halfWidth = Width / 2;
						if(Width % 2 == 1)
						{
							if(FMath::RandBool())
							{
								halfWidth++;
							}
						}
						NextRoad = currRoad + halfWidth;
						
						if(halfWidth >= MinBlockSide)
						{
							InnerRoadCuts.Add(NextRoad);
							MadeInnerCut = true;
							break; // it's the only cut we can get - no need to continue the cycle
						}
						else
						{
							// can't make a valid block by cutting in half
							break;
						}
					}
				}
				else
				{
					// not our first attempt - don't need to make a cut
					break;
				}
			}
		}
		else
		{
			// (NextRoad < Bound) -> false
			break;
		}
	}
	
	if(MadeInnerCut)
	{
		// if successfully made any cuts, apply them to blocks
		FBlock dividableBlock = blockToCut;
		
		
		for(int i = 0; i < InnerRoadCuts.Num(); i++)
		{
			TArray<FBlock> blocksAfterDivision;
			if(PerformOneBlockCut(dividableBlock, blocksAfterDivision, InnerRoadCuts[i], CutAcrossX))
			{
				check(blocksAfterDivision.Num() == 0 || blocksAfterDivision.Num() == 2);
				if(blocksAfterDivision.Num() != 0)
				{
					dividableBlock = blocksAfterDivision[1];
					Queue.Enqueue(blocksAfterDivision[0]);
					// blocksAfterDivision is created on each iteration - it will not persist on next iteration, no need to empty it
					AmountOfSuccessfulCuts++; // debug
				}
			}
			// else - we act as current InnerRoadCuts[i] hasn't existed and continue the cycle with the next cut
		}

		if(CheckValidResultingBlockWithFuturePossibleDivision(dividableBlock.GetBounds().X, dividableBlock.GetBounds().Y))
			Queue.Enqueue(dividableBlock);
		else
		{
			ResBlocks.Add(dividableBlock);
		}
		InnerRoadCuts.Empty();
	}
	else
	{
		// !MadeInnerCut
	}

	return MadeInnerCut;
}

bool AGenerator::PerformOneBlockCut(FBlock block, TArray<FBlock>& OutBlockPair, int32 CutCoord, bool CutAcrossX)
{
	// Make two blocks
	int32 FirstBlockEndX, FirstBlockEndY, SecondBlockStartX, SecondBlockStartY;

	if(CutAcrossX)
	{
		// New X
		FirstBlockEndX = CutCoord - 1;
		SecondBlockStartX = FirstBlockEndX + 2;
		// Same Y
		FirstBlockEndY = block.EndCorner.Y;
		SecondBlockStartY = block.StartCorner.Y;
	}
	else
	{
		// New Y
		FirstBlockEndY = CutCoord - 1;
		SecondBlockStartY = FirstBlockEndY + 2;
		// Same X
		FirstBlockEndX = block.EndCorner.X;
		SecondBlockStartX = block.StartCorner.X;
	}

	FIntVector FirstBlockStart = block.StartCorner;
	// FirstBlockEndX = CutAcrossX ? InnerRoadCuts[i] - 1 : rightCutSideBlock.EndCorner.X;
	// FirstBlockEndY = CutAcrossX ? rightCutSideBlock.EndCorner.Y : InnerRoadCuts[i] - 1;
	FIntVector FirstBlockEnd = FIntVector(FirstBlockEndX, FirstBlockEndY, block.EndCorner.Z);

	// SecondBlockEndX = CutAcrossX ? FirstBlockEnd.X + 2 : rightCutSideBlock.EndCorner.X;
	// SecondBlockEndY = CutAcrossX ? rightCutSideBlock.EndCorner.Y : FirstBlockEnd.Y + 2;
	FIntVector SecondBlockStart = FIntVector(SecondBlockStartX, SecondBlockStartY, block.StartCorner.Z);
	FIntVector SecondBlockEnd = block.EndCorner;

	FBlock firstBlock;
	firstBlock.SetParams(FirstBlockStart, FirstBlockEnd);
	// resultingBlocks.Add(firstBlock);
	FBlock secondBlock;
	secondBlock.SetParams(SecondBlockStart, SecondBlockEnd);

	if(CheckValidResultingBlockRestrictions(firstBlock)
		&& CheckValidResultingBlockRestrictions(secondBlock))
	{
		OutBlockPair.Empty();
		OutBlockPair.Add(firstBlock);
		OutBlockPair.Add(secondBlock);
		block = secondBlock;
	}
	else
	{
		OutBlockPair.Empty();
		return false;
	}

	// If new blocks are valid, make new road
	FRoad innerRoad;
	FIntVector StartRoadPoint;
	FIntVector EndRoadPoint;
	if(CutAcrossX)
	{
		// On intersections, roads lay one onto another, so we use coordinate not of a block, but of a road on its side
		StartRoadPoint = FIntVector(CutCoord, block.StartCorner.Y - 1, block.StartCorner.Z);
		EndRoadPoint = FIntVector(CutCoord, block.EndCorner.Y + 1, block.EndCorner.Z);
	}
	else
	{
		// On intersections, roads lay one onto another, so we use coordinate not of a block, but of a road on its side
		StartRoadPoint = FIntVector(block.StartCorner.X - 1, CutCoord, block.StartCorner.Z);
		EndRoadPoint = FIntVector(block.EndCorner.X + 1, CutCoord, block.EndCorner.Z);
	}
	innerRoad.Init(StartRoadPoint, EndRoadPoint, InnerRoadWidth, CutAcrossX);
	Roads.Add(innerRoad);

	UE_LOG(LogGeneration, Warning, TEXT("innerRoad: index in Roads[] = %d"), Roads.Num()-1);
	UE_LOG(LogGeneration, Warning, TEXT("StartRoadPoint = %d %d %d"), StartRoadPoint.X, StartRoadPoint.Y, StartRoadPoint.Z);
	UE_LOG(LogGeneration, Warning, TEXT("EndRoadPoint = %d %d %d"), EndRoadPoint.X, EndRoadPoint.Y, EndRoadPoint.Z);
	
	return true;
}

void AGenerator::FillEmptyCityBorderArea()
{
	FIntVector StartCityCoord;
	FIntVector EndCityCoord;

	StartCityCoord.X = XRoadPointsArray[0].coord;
	StartCityCoord.Y = YRoadPointsArray[0].coord;
	EndCityCoord.X = XRoadPointsArray[XRoadPointsArray.Num()-1].coord + XRoadPointsArray[XRoadPointsArray.Num()-1].RoadWidth - 1;
	EndCityCoord.Y = YRoadPointsArray[YRoadPointsArray.Num()-1].coord + YRoadPointsArray[YRoadPointsArray.Num()-1].RoadWidth - 1;

	ETileType emptyType = ETileType::ETT_NoCity;
	// ETileType emptyType = ETileType::ETT_Air;
	// Y start
	for(int y = 0; y < StartCityCoord.Y; y++)
	{
		for(int x = 0; x < WorldArray->Bounds.X; x++)
		{
			for(int z = 0; z < WorldArray->Bounds.Z; z++)
			{
				UWorldArrayItem* current = WorldArray->GetElement(z, y, x);
				current->bIsChosen = true;
				current->TileType = emptyType;
			}
		}
	}
	// Y end
	for(int y = EndCityCoord.Y + 1; y < WorldArray->Bounds.Y; y++)
	{
		for(int x = 0; x < WorldArray->Bounds.X; x++)
		{
			for(int z = 0; z < WorldArray->Bounds.Z; z++)
			{
				UWorldArrayItem* current = WorldArray->GetElement(z, y, x);
				current->bIsChosen = true;
				current->TileType = emptyType;
			}
		}
	}
	// X start
	for(int y = 0; y < WorldArray->Bounds.Y; y++)
	{
		for(int x = 0; x < StartCityCoord.X; x++)
		{
			for(int z = 0; z < WorldArray->Bounds.Z; z++)
			{
				UWorldArrayItem* current = WorldArray->GetElement(z, y, x);
				current->bIsChosen = true;
				current->TileType = emptyType;
			}
		}
	}
	// X end
	for(int y = 0; y < WorldArray->Bounds.Y; y++)
	{
		for(int x = EndCityCoord.X + 1; x < WorldArray->Bounds.X; x++)
		{
			for(int z = 0; z < WorldArray->Bounds.Z; z++)
			{
				UWorldArrayItem* current = WorldArray->GetElement(z, y, x);
				current->bIsChosen = true;
				current->TileType = emptyType;
			}
		}
	}
}

bool AGenerator::CheckValidResultingBlockRestrictions(FBlock block)
{
	bool TooShortX = FMath::Abs(block.EndCorner.X - block.StartCorner.X + 1) < (MinBlockSide);
	bool TooShortY = FMath::Abs(block.EndCorner.Y - block.StartCorner.Y + 1) < (MinBlockSide);
	
	if(TooShortX || TooShortY)
		return false;
	if(block.GetArea() < MinArea)
		return false;
	
	return true;
}

bool AGenerator::CheckDividableBlockRestrictions(FBlock block)
{
	bool TooShortX = FMath::Abs(block.EndCorner.X - block.StartCorner.X) < (BlockSideIsTooShortMultiplier * MinBlockSide);
	bool TooShortY = FMath::Abs(block.EndCorner.Y - block.StartCorner.Y) < (BlockSideIsTooShortMultiplier * MinBlockSide);
	bool AspectRatioIsLarge = block.GetAspectRatio() > MaxAspectRatio * AspectRatioLargeMultiplier;
	
	if((TooShortX || TooShortY) && AspectRatioIsLarge)
		return false;
	if(block.GetArea() < AreaLargeMultiplier * MinArea)
		return false;
	
	
	// float ParentAspectRatio = block.GetAspectRatio();
	// if(MaxAspectRatio / ParentAspectRatio < 2.f)
	// 	return false;

	return true;
}

bool AGenerator::CheckValidResultingBlockWithFuturePossibleDivision(int32 width, int32 length)
{
	if(length < width)
	{
		int32 temp = length;
		length = width;
		width = temp;
	}
	bool AreaIsLarge = width * length > MinArea * AreaLargeMultiplier;
	bool AspectRatioIsLarge =
		(float)length / width > MaxAspectRatio * AspectRatioLargeMultiplier;
	bool OneOfBlockSidesIsTooShort =
		width <= BlockSideIsTooShortMultiplier * MinBlockSide
	|| length <= BlockSideIsTooShortMultiplier * MinBlockSide;

	// TODO: проверить +1 \ -1 при вычислении площадей

	if(!(AreaIsLarge && AspectRatioIsLarge && OneOfBlockSidesIsTooShort))
		return true;
	else
		return false;
}




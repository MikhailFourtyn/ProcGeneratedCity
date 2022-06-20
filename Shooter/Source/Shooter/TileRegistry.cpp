// Fill out your copyright notice in the Description page of Project Settings.


#include "TileRegistry.h"

// Sets default values
ATileRegistry::ATileRegistry() :
RoadTags({
		ETileType::ETT_Road,
		ETileType::ETT_Road_Crossroads,
		ETileType::ETT_Road_OneLine,
		ETileType::ETT_Road_HalfOfWideRoad,
		ETileType::ETT_Road_Blank_Yellow_Side}),
BuildingTags({
	ETileType::ETT_Building,
	ETileType::ETT_Building_Door_Corner,
	ETileType::ETT_Building_Door_Section,
	ETileType::ETT_Building_Window_Corner,
	ETileType::ETT_Building_Window_Section,
	ETileType::ETT_Building_Greeble_Cube}),
SidewalkTags({
	ETileType::ETT_Sidewalks_Corner,
	ETileType::ETT_Sidewalks_Borderline,
	ETileType::ETT_Sidewalks_Inner
})
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATileRegistry::GetArrayOfCompatibleTilesByRelativeDeltaPosition(TArray<FTileCompatibilityElement>& OutArray, FTileRegistryEl RegistryRow,
	ETileCompatibilityDeltaPosition RelativeDeltaPosition)
{
	switch (RelativeDeltaPosition)
	{
	case ETileCompatibilityDeltaPosition::ETDP_OnForward:
		OutArray = RegistryRow.OnForwardCompatible;
		return;
	case ETileCompatibilityDeltaPosition::ETDP_OnBackward:
		OutArray = RegistryRow.OnBackCompatible;
		return;
	case ETileCompatibilityDeltaPosition::ETDP_OnTop:
		OutArray = RegistryRow.OnTopCompatible;
		return;
	case ETileCompatibilityDeltaPosition::ETDP_OnBottom:
		OutArray = RegistryRow.OnBottomCompatible;
		return;
	case ETileCompatibilityDeltaPosition::ETDP_OnLeft:
		OutArray = RegistryRow.OnLeftCompatible;
		return;
	case ETileCompatibilityDeltaPosition::ETDP_OnRight:
		OutArray = RegistryRow.OnRightCompatible;
		return;
	}
}

void ATileRegistry::GetArrayOfCompatibleTagsByRelativeDeltaPosition(TArray<FTagCompatibilityElement>& OutArray,
	FTagRegistryEl RegistryRow, ETileCompatibilityDeltaPosition RelativeDeltaPosition)
{
	switch (RelativeDeltaPosition)
	{
	case ETileCompatibilityDeltaPosition::ETDP_OnForward:
		OutArray = RegistryRow.OnForwardCompatible;
		return;
	case ETileCompatibilityDeltaPosition::ETDP_OnBackward:
		OutArray = RegistryRow.OnBackCompatible;
		return;
	case ETileCompatibilityDeltaPosition::ETDP_OnTop:
		OutArray = RegistryRow.OnTopCompatible;
		return;
	case ETileCompatibilityDeltaPosition::ETDP_OnBottom:
		OutArray = RegistryRow.OnBottomCompatible;
		return;
	case ETileCompatibilityDeltaPosition::ETDP_OnLeft:
		OutArray = RegistryRow.OnLeftCompatible;
		return;
	case ETileCompatibilityDeltaPosition::ETDP_OnRight:
		OutArray = RegistryRow.OnRightCompatible;
		return;
	}
}

ETileCompatibilityDeltaPosition ATileRegistry::GetRelativeDeltaPosition(ETileRotation MyRotation, ETileCompatibilityDeltaPosition WorldDeltaPosition)
{
	if(WorldDeltaPosition == ETileCompatibilityDeltaPosition::ETDP_OnTop)
		return ETileCompatibilityDeltaPosition::ETDP_OnTop;
	if(WorldDeltaPosition == ETileCompatibilityDeltaPosition::ETDP_OnBottom)
		return ETileCompatibilityDeltaPosition::ETDP_OnBottom;
	
	// TArray<ETileCompatibilityDeltaPosition> Positions = {
	// 	ETileCompatibilityDeltaPosition::ETDP_OnForward, // [0]=0
	// 	ETileCompatibilityDeltaPosition::ETDP_OnRight, // [1]=1
	// 	ETileCompatibilityDeltaPosition::ETDP_OnBackward, // [2]=2
	// 	ETileCompatibilityDeltaPosition::ETDP_OnLeft, // [3]=3
	// 	ETileCompatibilityDeltaPosition::ETDP_OnForward, // [4]=0
	// 	ETileCompatibilityDeltaPosition::ETDP_OnRight, // [5]=1
	// 	ETileCompatibilityDeltaPosition::ETDP_OnBackward, // [6]=2
	// 	ETileCompatibilityDeltaPosition::ETDP_OnLeft // [7]=3
	// 	// ETileCompatibilityDeltaPosition::ETDP_OnForward, // [8]=0
	// };
	
	const int IndexOfForward = 8;
	int CurrRotation = (int)MyRotation + 4;
	int WorldNeeded = (int)WorldDeltaPosition;
	int LocalDeltaPositionIndex = WorldNeeded + IndexOfForward - CurrRotation;
	// LocalDeltaPositionIndex is in margins [0, 7]
	// return Positions[LocalDeltaPositionIndex];
	LocalDeltaPositionIndex = LocalDeltaPositionIndex % 4; // LocalDeltaPositionIndex might be [1..7] of the commented array above, we need to move it to [0..3]
	// LocalDeltaPositionIndex is in margins [0, 3]
	return (ETileCompatibilityDeltaPosition)LocalDeltaPositionIndex;
}

bool ATileRegistry::IsCompatible(int MyRegIndex, ETileRotation MyTileRotation,
                                 int ComparableRegIndex, ETileRotation ComparableTileRotation,
                                 ETileCompatibilityDeltaPosition DeltaPositionToCompare)
{
	if(MyRegIndex >= RegistryArray.Num())
	{
		UE_LOG(LogGeneration, Error, TEXT("ATileRegistry::IsCompatible - MyRegIndex >= RegistryArray.Num()"));
		return false;
	}
	if(ComparableRegIndex >= RegistryArray.Num())
	{
		UE_LOG(LogGeneration, Error, TEXT("ATileRegistry::IsCompatible - ComparableRegIndex >= RegistryArray.Num()"));
		return false;
	}
	if(MyRegIndex < 0)
	{
		UE_LOG(LogGeneration, Error, TEXT("ATileRegistry::IsCompatible - MyRegIndex < 0"));
		return false;
	}
	if(ComparableRegIndex < 0)
	{
		UE_LOG(LogGeneration, Error, TEXT("ATileRegistry::IsCompatible - ComparableRegIndex < 0"));
		return false;
	}
	
	ETileType MyTag = RegistryArray[MyRegIndex].Tile.GetDefaultObject()->GetTileTypeTag();
	ETileType CompTag = RegistryArray[ComparableRegIndex].Tile.GetDefaultObject()->GetTileTypeTag();

	if((MyTag == ETileType::ETT_Air || MyTag == ETileType::ETT_NoCity)
		&& (CompTag == ETileType::ETT_Air || CompTag == ETileType::ETT_NoCity))
	{
		return true;
	}
	
	FTileRegistryEl MyRegistryRow = RegistryArray[MyRegIndex];

	// Get the array of compatible tiles corresponding to the position of ComparableTile,
	// respecting the turn of MyTile
	TArray<FTileCompatibilityElement> MyCompatibilityArray;
	ETileCompatibilityDeltaPosition RelativeDeltaPosition = GetRelativeDeltaPosition(MyTileRotation, DeltaPositionToCompare);
	GetArrayOfCompatibleTilesByRelativeDeltaPosition(MyCompatibilityArray, MyRegistryRow, RelativeDeltaPosition);
	
	ETileRotation RelativeRotation = GetRelativeRotationOfTheirTile(MyTileRotation,
		ComparableTileRotation, RelativeDeltaPosition);

	bool found = DoesCompatibilityArrayContainThisTile(MyCompatibilityArray, ComparableRegIndex, RelativeRotation);
	
	// Double-check by Tag compatibility
	if(!found)
	{
		// Check the tags compatibility array the same way
		int MyTagRowIndex = -1;
		
		for(int i = 0; i < TagRegistryArray.Num(); i++)
		{
			if(TagRegistryArray[i].TileTag == MyTag)
			{
				MyTagRowIndex = i;
			}
		}

		if(MyTagRowIndex != -1)
		{
			// Array of compatibility rules for my tag exists. Search it the same way we searched the RegistryArray for it
			FTagRegistryEl MyTagRow = TagRegistryArray[MyTagRowIndex];
			TArray<FTagCompatibilityElement> MyTagComp;
			GetArrayOfCompatibleTagsByRelativeDeltaPosition(MyTagComp, MyTagRow, RelativeDeltaPosition);
			found = DoesTagCompatibilityArrayContainThisTag(MyTagComp, CompTag, RelativeRotation);
		}
	}
	
	// Double-check for halves of wide road - if both are half, then they are compatible only with same tile
	if(found)
	{
		if(MyTag == ETileType::ETT_Road_HalfOfWideRoad)
		{
			if(MyTag == CompTag)
			{
				if(MyRegIndex == ComparableRegIndex)
					found = true;
				else
					found = false;
			}
		}
	}

	return found;
}

ETileCompatibilityDeltaPosition ATileRegistry::ReverseWorldDirection(ETileCompatibilityDeltaPosition direction) const
{
	switch (direction)
	{
	case ETileCompatibilityDeltaPosition::ETDP_OnBackward:
		return ETileCompatibilityDeltaPosition::ETDP_OnForward;
	case ETileCompatibilityDeltaPosition::ETDP_OnForward:
		return ETileCompatibilityDeltaPosition::ETDP_OnBackward;
		
	case ETileCompatibilityDeltaPosition::ETDP_OnLeft:
		return ETileCompatibilityDeltaPosition::ETDP_OnRight;
	case ETileCompatibilityDeltaPosition::ETDP_OnRight:
		return ETileCompatibilityDeltaPosition::ETDP_OnLeft;
		
	case ETileCompatibilityDeltaPosition::ETDP_OnTop:
		return ETileCompatibilityDeltaPosition::ETDP_OnBottom;
	case ETileCompatibilityDeltaPosition::ETDP_OnBottom:
		return ETileCompatibilityDeltaPosition::ETDP_OnTop;
		
	default:
		UE_LOG(LogGeneration, Error, TEXT("worldDirection has invalid value!"));
		return direction;
	}
}

ETileRotation ATileRegistry::GetRelativeRotationOfTheirTile(ETileRotation MyRotation, ETileRotation TheirRotation,
	ETileCompatibilityDeltaPosition TheirRelativePosition)
{
	// TArray<ETileRotation> Positions = {
	// 	ETileRotation::ETR_Forward, // [0]=0
	// 	ETileRotation::ETR_Right, // [1]=1
	// 	ETileRotation::ETR_Backward, // [2]=2
	// 	ETileRotation::ETR_Left, // [3]=3
	// 	ETileRotation::ETR_Forward, // [4]=0
	// 	ETileRotation::ETR_Right, // [5]=1
	// 	ETileRotation::ETR_Backward, // [6]=2
	// 	ETileRotation::ETR_Left // [7]=3
	// 	// ETileRotation::ETR_OnForward, // [8]=0
	// };

	int theirRotation = (int)TheirRotation - (int)MyRotation + 4;
	theirRotation = theirRotation % 4; // rotation might be [1..7] of the commented array above, we need to move it to [0..3]
	return (ETileRotation)theirRotation;
}

bool ATileRegistry::DoesCompatibilityArrayContainThisTile(TArray<FTileCompatibilityElement> CompatibilityArray, int TileIndexInRegister, ETileRotation RelativeRotation)
{
	for(int i = 0; i < CompatibilityArray.Num(); i++)
	{
		if(CompatibilityArray[i].Rotation == RelativeRotation)
		{
			int curr = CompatibilityArray[i].Tile.GetDefaultObject()->GetIndexInRegister();
			if(curr == TileIndexInRegister)
				return true;
		}
	}
	return false;
}

bool ATileRegistry::DoesTagCompatibilityArrayContainThisTag(TArray<FTagCompatibilityElement> CompatibilityArray,
	ETileType Tag, ETileRotation RelativeRotation)
{
	for(int i = 0; i < CompatibilityArray.Num(); i++)
	{
		if(CompatibilityArray[i].Tag == Tag && CompatibilityArray[i].Rotation == RelativeRotation)
		{
			return true;
		}
	}
	return false;
}

void ATileRegistry::Init()
{
	// Add Additional Tiles to RegistryArray
	if(AdditionalTilesList.Num() > 0)
	{
		for(int addTile = 0; addTile < AdditionalTilesList.Num(); addTile++)
		{
			if(AdditionalTilesList[addTile] != nullptr)
			{
				// Search the RegistryArray, if it already includes current additional tile
				bool alreadyExists = false;

				for(int regTile = 0; regTile < RegistryArray.Num(); regTile++)
				{
					if(RegistryArray[regTile].Tile.GetDefaultObject()->GetName()
						== AdditionalTilesList[addTile].GetDefaultObject()->GetName())
					{
						alreadyExists = true;
						break;
					}
				}

				// If current additional tile doesn't exist yet, add to RegistryArray
				if(!alreadyExists)
				{
					FTileRegistryEl newRegistryEl;
					newRegistryEl.Tile = AdditionalTilesList[addTile];
					newRegistryEl.TileInstance = nullptr;
					newRegistryEl.OnBackCompatible = TArray<FTileCompatibilityElement>();
					newRegistryEl.OnForwardCompatible = TArray<FTileCompatibilityElement>();
					newRegistryEl.OnLeftCompatible = TArray<FTileCompatibilityElement>();
					newRegistryEl.OnRightCompatible = TArray<FTileCompatibilityElement>();
					newRegistryEl.OnTopCompatible = TArray<FTileCompatibilityElement>();
					newRegistryEl.OnBottomCompatible = TArray<FTileCompatibilityElement>();

					RegistryArray.Add(newRegistryEl);
				}
			}
		}
	}

	// Fill superposition arrays from RegistryArray
	for(int i = 0; i < RegistryArray.Num(); i++)
	{
		if(RegistryArray[i].Tile != nullptr)
		{
			// Init the pointer to singleton instances of current tile and of compatible tiles
			InitInstancePointersInRegistryRow(RegistryArray[i]);
			RegistryArray[i].TileInstance->SetIndexInRegister(i);

			TArray<ETileRotation> PossibleRotations = RegistryArray[i].TileInstance->GetPossibleTileRotations();
			int Weight = RegistryArray[i].TileInstance->GetWeight();

			for(int k = 0; k < PossibleRotations.Num(); k++)
			{
				FWorldArrayWFCSuperpositionElement El = { i, PossibleRotations[k], Weight };

				// Fill array of shortened superposition - Roads
				if(IsTileARoad(RegistryArray[i].TileInstance))
				{
					// RoadIndexes.AddUnique(i);
					RoadSuperpositionArray.Add(El);
				}
				if(IsTileARoadCrossroads(RegistryArray[i].TileInstance))
				{
					RoadCrossroadsSuperpositionArray.Add(El);
				}
				if(IsTileARoadOneLine(RegistryArray[i].TileInstance))
				{
					RoadOneLineSuperpositionArray.Add(El);
				}
				if(IsTileARoadDouble(RegistryArray[i].TileInstance))
				{
					RoadDoubleSuperpositionArray.Add(El);
				}
				
				// Fill array of shortened superposition - Sidewalks
				if(IsTileASidewalkInner(RegistryArray[i].TileInstance))
				{
					SidewalkInnerSuperpositionArray.Add(El);
				}
				if(IsTileASidewalkBorderline(RegistryArray[i].TileInstance))
				{
					SidewalkBorderlineSuperpositionArray.Add(El);
				}
				if(IsTileASidewalkCorner(RegistryArray[i].TileInstance))
				{
					SidewalkCornerSuperpositionArray.Add(El);
				}

				if(IsTileABuilding(RegistryArray[i].TileInstance))
				{
					BuildingSuperpositionArray.Add(El);
				}

				if(RegistryArray[i].TileInstance->GetTileTypeTag() == ETileType::ETT_Building_Door_Corner)
				{
					BuildingDoorCornerSuperpositionArray.Add(El);
				}
				if(RegistryArray[i].TileInstance->GetTileTypeTag() == ETileType::ETT_Building_Door_Section)
				{
					BuildingDoorSectionSuperpositionArray.Add(El);
				}
				if(RegistryArray[i].TileInstance->GetTileTypeTag() == ETileType::ETT_Building_Window_Corner)
				{
					BuildingWindowCornerSuperpositionArray.Add(El);
				}
				if(RegistryArray[i].TileInstance->GetTileTypeTag() == ETileType::ETT_Building_Window_Section)
				{
					BuildingWindowSectionSuperpositionArray.Add(El);
				}
				if(RegistryArray[i].TileInstance->GetTileTypeTag() == ETileType::ETT_Building_Greeble_Cube)
				{
					BuildingGreebleSuperpositionArray.Add(El);
				}

				if(IsTileAir(RegistryArray[i].TileInstance))
				{
					AirSuperpositionArray.Add(El);
				}
				
				// Fill the array of full superposition
				FullSuperpositionArray.Add(El);
			}
		}
	}
}

// Called when the game starts or when spawned
void ATileRegistry::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATileRegistry::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<FWorldArrayWFCSuperpositionElement> ATileRegistry::GetSuperpositionArrayByTag(ETileType Tag) const
{
	switch (Tag)
	{
	case ETileType::ETT_Road_Crossroads:
		return RoadCrossroadsSuperpositionArray;
	case ETileType::ETT_Road_OneLine:
		return RoadOneLineSuperpositionArray;
	case ETileType::ETT_Road_HalfOfWideRoad:
		return RoadDoubleSuperpositionArray;
	case ETileType::ETT_Road:
		return RoadSuperpositionArray;
	
	case ETileType::ETT_Sidewalks_Inner:
		return SidewalkInnerSuperpositionArray;
	case ETileType::ETT_Sidewalks_Borderline:
		return SidewalkBorderlineSuperpositionArray;
	
	case ETileType::ETT_Building_Window_Corner:
		return BuildingWindowCornerSuperpositionArray;
	case ETileType::ETT_Building_Window_Section:
		return BuildingWindowSectionSuperpositionArray;
	case ETileType::ETT_Building_Door_Corner:
		return BuildingDoorCornerSuperpositionArray;
	case ETileType::ETT_Building_Door_Section:
		return BuildingDoorSectionSuperpositionArray;
	case ETileType::ETT_Building_Greeble_Cube:
		return BuildingGreebleSuperpositionArray;
		
	case ETileType::ETT_Building:
		return BuildingSuperpositionArray;

	case ETileType::ETT_Air:
	case ETileType::ETT_NoCity:
		return AirSuperpositionArray;
		
	case ETileType::ETT_Undefined:
	case ETileType::ETT_MAX:
	default:
		return FullSuperpositionArray;
	}
}

int ATileRegistry::GetRandomWeightedTileIndex(TArray<FWorldArrayWFCSuperpositionElement> Tiles)
{
	if(Tiles.Num() == 0)
	{
		UE_LOG(LogGeneration, Error, TEXT("UWFCGeneratorComponent::GetRandomWeightedTileIndex() - Tiles.Num() = 0!"));
		return -1;
	}
	
	TArray<int> Values;
	Values.Init(0, Tiles.Num());

	Values[0] = Tiles[0].Weight;
	for(int i = 1; i < Tiles.Num(); i++)
	{
		Values[i] = Values[i-1] + Tiles[i].Weight;
	}

	int RandValue = FMath::RandRange(1, Values[Values.Num()-1]);

	for(int i = 0; i < Values.Num(); i++)
	{
		if(RandValue < Values[i])
		{
			return i;	
		}
	}
	
	return 0;
}

void ATileRegistry::InitInstancePointersInRegistryRow(FTileRegistryEl& Row)
{
	check(Row.Tile != nullptr);
	
	Row.TileInstance = Row.Tile.GetDefaultObject();

	// Init the pointers to singleton instances of compatible tiles

	InitInstancePointersInCompatibilityArray(Row.OnForwardCompatible);
	InitInstancePointersInCompatibilityArray(Row.OnBackCompatible);
	InitInstancePointersInCompatibilityArray(Row.OnLeftCompatible);
	InitInstancePointersInCompatibilityArray(Row.OnRightCompatible);
	InitInstancePointersInCompatibilityArray(Row.OnTopCompatible);
	InitInstancePointersInCompatibilityArray(Row.OnBottomCompatible);
}

void ATileRegistry::InitInstancePointersInCompatibilityArray(TArray<FTileCompatibilityElement> &Array)
{
	for(int k = 0; k < Array.Num(); k++)
	{
		check(Array[k].Tile != nullptr);
		
		Array[k].TileInstance = Array[k].Tile.GetDefaultObject();
	}
}

bool ATileRegistry::IsTileARoad(const ATile* Tile)
{
	return CheckTileTypeTags(Tile, RoadTags);
}

bool ATileRegistry::IsTileARoadCrossroads(const ATile* Tile)
{
	return Tile->GetTileTypeTag() == ETileType::ETT_Road_Crossroads;
}

bool ATileRegistry::IsTileARoadOneLine(const ATile* Tile)
{
	return Tile->GetTileTypeTag() == ETileType::ETT_Road_OneLine;
}

bool ATileRegistry::IsTileARoadDouble(const ATile* Tile)
{
	return Tile->GetTileTypeTag() == ETileType::ETT_Road_HalfOfWideRoad;
}

bool ATileRegistry::IsTileASidewalk(const ATile* Tile)
{
	return CheckTileTypeTags(Tile, SidewalkTags);
}

bool ATileRegistry::IsTileASidewalkInner(const ATile* Tile)
{
	return Tile->GetTileTypeTag() == ETileType::ETT_Sidewalks_Inner;
}

bool ATileRegistry::IsTileASidewalkBorderline(const ATile* Tile)
{
	return Tile->GetTileTypeTag() == ETileType::ETT_Sidewalks_Borderline;
}

bool ATileRegistry::IsTileASidewalkCorner(const ATile* Tile)
{
	return Tile->GetTileTypeTag() == ETileType::ETT_Sidewalks_Corner;
}

bool ATileRegistry::IsTileABuilding(const ATile* Tile)
{
	return CheckTileTypeTags(Tile, BuildingTags);
}

bool ATileRegistry::IsTileAir(const ATile* Tile)
{
	return Tile->GetTileTypeTag() == ETileType::ETT_Air;
}

bool ATileRegistry::CheckTileTypeTags(const ATile* Tile, const TArray<ETileType>& TypeTags) const
{
	ETileType TileTag = Tile->GetTileTypeTag();
	
	if(TypeTags.Num() == 0)
		return false;
	
	for(int i = 0; i < TypeTags.Num(); i++)
	{
		if(TypeTags[i] == TileTag)
		{
			return true;
		}
	}
	return false;
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "WFCGeneratorComponent.h"

// Sets default values for this component's properties
UWFCGeneratorComponent::UWFCGeneratorComponent() :
bDebugWFCOnlyFloor(true),
MaxAttempts(100)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

bool UWFCGeneratorComponent::Generate(UWorldItem3DArray* worldArray, TArray<UWorldArrayItem*>& OutWorldArray)
{
	if(!TileRegistryActor)
		return false;

	UWorldItem3DArray* InputWorldArray = NewObject<UWorldItem3DArray>();
	InputWorldArray->Bounds = worldArray->Bounds;
	InputWorldArray->Arr = CopyWorldArray(worldArray->Arr, worldArray->Arr.Num());

	ReservedWorldArray = NewObject<UWorldItem3DArray>();
	
	ReservedWorldArray->Bounds = worldArray->Bounds;
	if(bDebugWFCOnlyFloor)
	{
		int floorNum = worldArray->Bounds.X * worldArray->Bounds.Y;
		ReservedWorldArray->Bounds.Z = 1;
		ReservedWorldArray->Arr = CopyWorldArray(worldArray->Arr, floorNum);
	}
	else
	{
		ReservedWorldArray->Arr = CopyWorldArray(worldArray->Arr, worldArray->Arr.Num());
	}
	ReservedWorldArray->DefaultValue = worldArray->DefaultValue;
	SetSuperpositionsOfArrayElementsByType(*ReservedWorldArray);

	bool generatedSuccessfully = StartWFC();
	
	if(!generatedSuccessfully)
	{
		UE_LOG(LogGeneration, Error, TEXT("WFC FAIL! Attempts: %d"), MaxAttempts);
	}

	int32 inputArrBounds = InputWorldArray->Bounds.Z * InputWorldArray->Bounds.Y * InputWorldArray->Bounds.X;
	int32 wfcArrBounds = WfcWorldArray->Bounds.Z * WfcWorldArray->Bounds.Y * WfcWorldArray->Bounds.X;

	TArray<UWorldArrayItem*> returnArr;
	for(int i = 0; i < inputArrBounds; i++)
	{
		UWorldArrayItem* item = NewObject<UWorldArrayItem>();
		item->Init(WfcWorldArray->Arr[i]->TileType, WfcWorldArray->Arr[i]->ChosenTileIndex,
			WfcWorldArray->Arr[i]->PossibleTiles, WfcWorldArray->Arr[i]->TileRotation,
			WfcWorldArray->Arr[i]->bIsChosen);

		returnArr.Add(item);
	}
	
	// OutWorldArray = CopyWorldArray(WfcWorldArray->Arr, WfcWorldArray->Arr.Num());

	OutWorldArray = returnArr;

	return generatedSuccessfully;
}

TArray<UWorldArrayItem*> UWFCGeneratorComponent::CopyWorldArray(TArray<UWorldArrayItem*> inputArray, int32 Num)
{
	TArray<UWorldArrayItem*> newArr;
	for(int i = 0; i < Num; i++)
	{
		UWorldArrayItem* item = NewObject<UWorldArrayItem>();
		
		item->Init(inputArray[i]->TileType, inputArray[i]->ChosenTileIndex,
			inputArray[i]->PossibleTiles, inputArray[i]->TileRotation,
			inputArray[i]->bIsChosen);

		newArr.Add(item);
	}
	return newArr;
}


// Called when the game starts
void UWFCGeneratorComponent::BeginPlay()
{
	Super::BeginPlay();

	if(TileRegistryClass)
	{
		TileRegistryActor = GetWorld()->SpawnActor<ATileRegistry>(TileRegistryClass);
		
		if(TileRegistryActor)
		{
			TileRegistryActor->Init();
		}
	}
}


void UWFCGeneratorComponent::SetSuperpositionsOfArrayElementsByType(UWorldItem3DArray& worldArray)
{
	check(TileRegistryActor);

	for(int i = 0; i < worldArray.Arr.Num(); i++)
	{
		if(!worldArray.Arr[i]->bIsChosen)
		{
			worldArray.Arr[i]->PossibleTiles =
				TileRegistryActor->GetSuperpositionArrayByTag(worldArray.Arr[i]->TileType);
		}
	}
}

bool UWFCGeneratorComponent::StartWFC()
{
	WfcWorldArray = NewObject<UWorldItem3DArray>();
	WfcWorldArray->Bounds = ReservedWorldArray->Bounds;
	WfcWorldArray->DefaultValue = ReservedWorldArray->DefaultValue;

	if(bDebugWFCOnlyFloor)
	{
		WfcWorldArray->Bounds.Z = 1;
		int WfcMapLen = WfcWorldArray->Bounds.X * WfcWorldArray->Bounds.Y;

		for(int i = 0; i < WfcMapLen; i++)
		{
			WfcWorldArray->Arr.Emplace(ReservedWorldArray->Arr[i]);
		}
	}

	currentMaxSuperposition = 1;
	int WFC_MaxAttempts = MaxAttempts;
	int WFC_Attempts = 0;

	bool WFCFinished = false;
	while(!WFCFinished && WFC_Attempts++ < WFC_MaxAttempts)
	{
		if(WFC_Attempts > 0)
			ResetWFCWorldMap();

		TQueue<int> CoordsToPropagateQueue;
		int32 slotIndexToCollapse = ChooseSlotToStartWFC();

		while(slotIndexToCollapse >= 0)
		{
			Collapse(CoordsToPropagateQueue, slotIndexToCollapse);
			bool metContradiction = false;
			while(!CoordsToPropagateQueue.IsEmpty() && !metContradiction)
			{
				metContradiction = !Propagate(CoordsToPropagateQueue);
			}

			if(metContradiction)
			{
				// Attempt again
				break;
			}

			// Observation:
			slotIndexToCollapse = FindSlotWithLeastChoice(bDebugWFCOnlyFloor);
			if(slotIndexToCollapse < 0)
			{
				// haven't found any suitable slot
				UE_LOG(LogGeneration, Display, TEXT("UWFCGeneratorComponent::StartWFC() - WFC finished!"));
				WFCFinished = true;
				break;
			}
		}
	}
	return WFCFinished;
}

int32 UWFCGeneratorComponent::ChooseSlotToStartWFC()
{
	// Start from the start of first road
	// return Roads[0].StartPoint.Y * WfcWorldArray->Bounds.X + Roads[0].StartPoint.X;
	return FindSlotWithLeastChoice(true);
}

void UWFCGeneratorComponent::ResetWFCWorldMap()
{
	WfcWorldArray->Arr.Empty();
	WfcWorldArray->Bounds = ReservedWorldArray->Bounds;
	WfcWorldArray->Arr = CopyWorldArray(ReservedWorldArray->Arr, ReservedWorldArray->Arr.Num());
}

int32 UWFCGeneratorComponent::FindSlotWithLeastChoice(bool bCheckOnlyFloor)
{
	int32 ZBound = WfcWorldArray->Bounds.Z;
	if(bCheckOnlyFloor)
		ZBound = 1;

	int32 maxSuperposition = -1;
	int32 minSuperposition = INT_MAX;
	int32 chosenSlot = -1;

	int z = 0;

	while(z++ < ZBound && chosenSlot == -1)
	{
		for(int y = 0; y < WfcWorldArray->Bounds.Y; y++)
			for(int x = 0; x < WfcWorldArray->Bounds.X; x++)
			{
				int32 Index = WfcWorldArray->GetLinearIndex(z, y, x);
				int32 superpos = WfcWorldArray->Arr[Index]->PossibleTiles.Num();
				if(superpos < minSuperposition)
				{
					minSuperposition = superpos;
					chosenSlot = Index;
				}
				if(superpos > maxSuperposition)
				{
					maxSuperposition = superpos;
				}
			}
	}

	if(maxSuperposition != -1)
		currentMaxSuperposition = maxSuperposition;

	if(minSuperposition == 0)
	{
		UE_LOG(LogGeneration, Error, TEXT("MinSuperposition = 0!"));
	}

	return chosenSlot;
}

void UWFCGeneratorComponent::Collapse(TQueue<int>& CoordsQueue, int OutIndex)
{
	int32 tile = TileRegistryActor->GetRandomWeightedTileIndex(WfcWorldArray->Arr[OutIndex]->PossibleTiles);
	bool shouldCheckNeighbours = WfcWorldArray->Arr[OutIndex]->PossibleTiles.Num() > 1;

	FWorldArrayWFCSuperpositionElement Element = WfcWorldArray->Arr[OutIndex]->PossibleTiles[tile];
	WfcWorldArray->Arr[OutIndex]->ChooseSpecificTile(Element);

	if(shouldCheckNeighbours)
	{
		EnqueueNeighbours(CoordsQueue, OutIndex);
	}
}

bool UWFCGeneratorComponent::Propagate(TQueue<int>& CoordsQueue)
{
	int current;
	bool currentSlotIsUnchanged = true;
	bool successDeque = CoordsQueue.Dequeue(current);

	int debug_contradiction = -1;
	TArray<FString> debug_contr_pos;
	
	if(successDeque)
	{
		// If the tile is already chosen, no need to change
		if(!WfcWorldArray->Arr[current]->bIsChosen)
		{
			// CHECK COMPATIBILITY FOR EACH IN PossibleTiles
			for(int i = 0; i < WfcWorldArray->Arr[current]->PossibleTiles.Num(); i++)
			{
				debug_contr_pos.Empty();
				FWorldArrayWFCSuperpositionElement currentPossibleTile = WfcWorldArray->Arr[current]->PossibleTiles[i];
				// CAN THIS TILE BE SET HERE? IF ANY SIDE DOES NOT CONTAIN A COMPATIBLE TILE => DELETE THIS TILE FROM PossibleTiles AND EnqueueNeighbours()

				bool fitsByForw = TileFitsByDirection(current, currentPossibleTile, ETileCompatibilityDeltaPosition::ETDP_OnForward);
				bool fitsByBack = TileFitsByDirection(current, currentPossibleTile, ETileCompatibilityDeltaPosition::ETDP_OnBackward);
				bool fitsByLeft = TileFitsByDirection(current, currentPossibleTile, ETileCompatibilityDeltaPosition::ETDP_OnLeft);
				bool fitsByRight = TileFitsByDirection(current, currentPossibleTile, ETileCompatibilityDeltaPosition::ETDP_OnRight);
				bool fitsByTop = TileFitsByDirection(current, currentPossibleTile, ETileCompatibilityDeltaPosition::ETDP_OnTop);
				bool fitsByBot = TileFitsByDirection(current, currentPossibleTile, ETileCompatibilityDeltaPosition::ETDP_OnBottom);
				
				if(!fitsByForw
					|| !fitsByBack
					|| !fitsByLeft
					|| !fitsByRight
					|| !fitsByTop
					|| !fitsByBot
					)
				{
					
					// If current tile doesn't fit this slot, remove it
					currentSlotIsUnchanged = false;
					debug_contradiction = WfcWorldArray->Arr[current]->PossibleTiles[i].TileIndexInRegister;
					WfcWorldArray->Arr[current]->PossibleTiles.RemoveAt(i);
					i--; // we remove an element, next tile gets into the position of current one, which was already processed
				}
			}

			if(WfcWorldArray->Arr[current]->PossibleTiles.Num() == 0)
			{
				// We met a contradiction! Process it outside of this function
				UE_LOG(LogGeneration, Error, TEXT("WFC - Met a contradiction"));
				FString debug_contr_name = TileRegistryActor->RegistryArray[debug_contradiction].TileInstance->GetName();
				UE_LOG(LogGeneration, Error, TEXT("%s"), *debug_contr_name);
				for(int p = 0; p < debug_contr_pos.Num(); p++)
				{
					UE_LOG(LogGeneration, Error, TEXT("%s"), *debug_contr_pos[p]);
				}
				UE_LOG(LogGeneration, Error, TEXT("WFC - Met a contradiction END"));
				

				return false;
			}
			
			if(WfcWorldArray->Arr[current]->PossibleTiles.Num() == 1)
			{
				// Мы выбрали единственно возможный тайл для данного слота
				
				WfcWorldArray->Arr[current]->ChooseSpecificTile(WfcWorldArray->Arr[current]->PossibleTiles[0]);
				
			}

			if(!currentSlotIsUnchanged)
			{
				EnqueueNeighbours(CoordsQueue, current);
			}
		}
	}
	else
	{
		UE_LOG(LogGeneration, Warning, TEXT("UWFCGeneratorComponent::Collapse cant deque!"));
	}
	return true;
}

void UWFCGeneratorComponent::EnqueueNeighbours(TQueue<int>& CoordsQueue, int current)
{
	TArray<int> IndexesToCheck =
		{
		current + (WfcWorldArray->Bounds.Y * WfcWorldArray->Bounds.X), // +Z
		current - (WfcWorldArray->Bounds.Y * WfcWorldArray->Bounds.X), // -Z
		current + WfcWorldArray->Bounds.X, // +Y
		current - WfcWorldArray->Bounds.X, // -Y
		current + 1, // +X
		current - 1 // -X	
	};
		
	for(int i = 0; i < IndexesToCheck.Num(); i++)
	{
		if(WfcWorldArray->Arr.IsValidIndex(IndexesToCheck[i]))
		{
			if(!WfcWorldArray->Arr[IndexesToCheck[i]]->bIsChosen
				&& WfcWorldArray->Arr[IndexesToCheck[i]]->TileType != ETileType::ETT_Air
				&& WfcWorldArray->Arr[IndexesToCheck[i]]->TileType != ETileType::ETT_NoCity)
			{
				CoordsQueue.Enqueue(IndexesToCheck[i]);
			}
		}
	}
}

bool UWFCGeneratorComponent::TileFitsByDirection(int32 currentIndexInWorld, FWorldArrayWFCSuperpositionElement currentPossibleTile, ETileCompatibilityDeltaPosition worldDirection)
{
	int32 secondIndex = currentIndexInWorld + WfcWorldArray->GetDeltaIndex(worldDirection);

	if(!WfcWorldArray->Arr.IsValidIndex(secondIndex))
	{
		// If index is not valid, then there's no error in compatibility at the border of the array
		return true;
	}

	if(WfcWorldArray->Arr[currentIndexInWorld]->TileType == ETileType::ETT_NoCity
		|| WfcWorldArray->Arr[secondIndex]->TileType == ETileType::ETT_NoCity)
	{
		// If there's no city, then it's a border and we don't need to compare it with anything
		return true;
	}
	
	// Check all the tiles in the direction
	int32 MyRegisterIndex = currentPossibleTile.TileIndexInRegister;
	ETileRotation MyWorldRotation = currentPossibleTile.Rotation;

	int32 max = WfcWorldArray->Arr[secondIndex]->PossibleTiles.Num();
	for(int i = 0; i < max; i++)
	{
		int32 ComparableRegisterIndex = WfcWorldArray->Arr[secondIndex]->PossibleTiles[i].TileIndexInRegister;
		ETileRotation ComparableWorldRotation = WfcWorldArray->Arr[secondIndex]->PossibleTiles[i].Rotation;

		// check compatibility
		// with reverse check - by the world direction from 2nd tile to 1st
		// so we check both tile compatibility arrays
		// and avoid the human error of setting one tile compatible with another but not vice versa
		if(TileRegistryActor->IsCompatible(
			MyRegisterIndex, MyWorldRotation,
			ComparableRegisterIndex, ComparableWorldRotation,
			worldDirection
			)
			||
			TileRegistryActor->IsCompatible(
			ComparableRegisterIndex, ComparableWorldRotation,
			MyRegisterIndex, MyWorldRotation,
			TileRegistryActor->ReverseWorldDirection(worldDirection))
			)
		{
			// we need at least one compatible - no need to check the full array
			return true;
		}
	}

	return false;
}
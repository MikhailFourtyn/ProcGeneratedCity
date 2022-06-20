// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorldItem3DArray.h"
#include "TileRegistry.h"
#include "TileCompatibilityDeltaPosition.h"
#include "WorldArrayWFCSuperpositionElement.h"
#include "WFCGeneratorComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UWFCGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWFCGeneratorComponent();

	bool Generate(UWorldItem3DArray* worldArray, TArray<UWorldArrayItem*>& OutWorldArray);

	TArray<UWorldArrayItem*> CopyWorldArray(TArray<UWorldArrayItem*> inputArray, int32 Num);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bDebugWFCOnlyFloor;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	void SetSuperpositionsOfArrayElementsByType(UWorldItem3DArray& worldArray);

	bool StartWFC();

	int32 ChooseSlotToStartWFC();

	void ResetWFCWorldMap();

	// Returns 1D-index in world 3D-array of an element with minimum superposition
	// If haven't found any suitable element, returns -1
	// Also fills in value of currentMaxSuperposition
	int32 FindSlotWithLeastChoice(bool bCheckOnlyFloor);

	void Collapse(TQueue<int>& CoordsQueue, int OutIndex);

	// Propagates the changes of collapse
	// Checks if a slot from CoordsQueue is compatible with the possible tiles in adjacent slots
	// Returns false if we met a contradiction, otherwise returns true
	bool Propagate(TQueue<int>& CoordsQueue);

	// Adds neighbour coordinates of current coordinate to WFC queue. Avoids Air and NoCity tiles
	void EnqueueNeighbours(TQueue<int>& CoordsQueue, int current);
	
	// If any (at least 1) possible tile in the slot by the provided direction is compatible with current possible tile, returns true
	// If there is no slot in provided direction, returns true
	// If no possible tile in the slot by the provided direction is compatible with current possible tile, returns false
	bool TileFitsByDirection(int32 currentIndexInWorld, FWorldArrayWFCSuperpositionElement currentPossibleTile,
		ETileCompatibilityDeltaPosition direction);
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Tiles)
	TSubclassOf<ATileRegistry> TileRegistryClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Tiles)
	ATileRegistry* TileRegistryActor;

	int32 currentMaxSuperposition;

	UPROPERTY()
	UWorldItem3DArray* ReservedWorldArray;

	UPROPERTY()
	UWorldItem3DArray* WfcWorldArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin=1))
	int32 MaxAttempts;

public:
	FORCEINLINE ATileRegistry* GetTileRegistryActor() const { return TileRegistryActor; }
};

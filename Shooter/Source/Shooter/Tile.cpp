// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"

// Sets default values
ATile::ATile() :
bIsPivotInCenter(false),
TileBounds(FVector(0)),
bUseFullMeshBounds(false),
Rotatibility(ETileRotatibility::ETR_FourRotations),
Weight(100),
TileTypeTag(ETileType::ETT_Undefined),
ColorTag(ETileColorTag::ETCT_Indifferent)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(SceneRoot);

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Main Mesh"));
	MainMesh->SetupAttachment(RootComponent);

	check(IsValid(MainMesh));
	if(bUseFullMeshBounds)
		TileBounds = MainMesh->Bounds.BoxExtent;
	else
		TileBounds = FVector(
		((int32)(MainMesh->Bounds.BoxExtent.X+1.f)) / 100 * 100,
		((int32)(MainMesh->Bounds.BoxExtent.Y+1.f)) / 100 * 100,
		((int32)(MainMesh->Bounds.BoxExtent.Z+1.f)) / 100 * 100);
	// +1.f here is for: if we have 499.5f, we want it to be 500
}

TArray<ETileRotation> ATile::GetPossibleTileRotations() const
{
	check(Rotatibility != ETileRotatibility::ETR_Undefined);

	TArray<ETileRotation> Rotations;

	if(Rotatibility == ETileRotatibility::ETR_Indifferent)
	{
		Rotations.Add(ETileRotation::ETR_Forward);
	}
	else if(Rotatibility == ETileRotatibility::ETR_TwoRotations)
	{
		Rotations.Add(ETileRotation::ETR_Forward);
		Rotations.Add(ETileRotation::ETR_Left);
	}
	else if(Rotatibility == ETileRotatibility::ETR_FourRotations)
	{
		Rotations.Add(ETileRotation::ETR_Forward);
		Rotations.Add(ETileRotation::ETR_Left);
		Rotations.Add(ETileRotation::ETR_Right);
		Rotations.Add(ETileRotation::ETR_Backward);
	}

	return Rotations;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int ATile::GetIndexInRegister() const
{
	if(IndexInRegister < 0)
		UE_LOG(LogGeneration, Error, TEXT("ATile::GetIndexInRegister() - IndexInRegister < 0 !"));

	return IndexInRegister;
}

void ATile::SetIndexInRegister(int Value)
{
	if(Value < 0)
		UE_LOG(LogGeneration, Error, TEXT("ATile::SetIndexInRegister - Value < 0 !"));

	IndexInRegister = Value;
}


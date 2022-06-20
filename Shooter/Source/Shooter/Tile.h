// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "TileType.h"
#include "GenerationLogs.h"
#include "TileColorTag.h"
#include "TileRotation.h"
#include "TileCompatibilityElement.h"
#include "Tile.generated.h"

UCLASS()
class SHOOTER_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	TArray<ETileRotation> GetPossibleTileRotations() const;
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneRoot;
	
	// Mesh of current Tile
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MainMesh;

	// Stright area of the tile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector TileBounds;

	// True if pivot is in center
	// False if pivot is at the corner
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsPivotInCenter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bUseFullMeshBounds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ETileType TileTypeTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ETileColorTag ColorTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ETileRotatibility Rotatibility;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<ETileSymmetry> Symmetry;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin=1, ClampMax=100))
	int Weight;
	
	int IndexInRegister = -1;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int GetIndexInRegister() const;
	void SetIndexInRegister(int Value);
	
	FORCEINLINE int GetWeight() const { return Weight; }
	FORCEINLINE ETileType GetTileTypeTag() const { return TileTypeTag; }
	FORCEINLINE ETileColorTag GetColorTag() const { return ColorTag; }

};

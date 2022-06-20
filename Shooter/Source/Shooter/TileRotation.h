#pragma once

#include "CoreMinimal.h"
#include "TileRotation.generated.h"

UENUM()
enum class ETileRotation: uint8
{
	// Along X+
	ETR_Forward = 0 UMETA(DisplayName = "Forward"),
	// Along Y+
	ETR_Right = 1 UMETA(DisplayName = "Right"),
	// Along X-
	ETR_Backward = 2 UMETA(DisplayName = "Backward"),
	// Along Y-
	ETR_Left = 3 UMETA(DisplayName = "Left"),
	/*
	// Along X
	ETR_Vertical UMETA(DisplayName = "Vertical"),
	// Along Y
	ETR_Horizontal UMETA(DisplayName = "Horizontal"),
	// Doesn't matter: compatibles from each side are exact same
	ETR_Indifferent UMETA(DisplayName = "Indifferent"),
	*/
	
	ETR_Undefined UMETA(DisplayName = "Undefined"),
	ETR_MAX UMETA(DisplayName = "Default MAX")
};

UENUM()
enum class ETileRotatibility: uint8
{
	ETR_Undefined UMETA(DisplayName = "Undefined"),
	ETR_FourRotations UMETA(DisplayName = "Four Rotations"),
	ETR_TwoRotations UMETA(DisplayName = "Two Rotations"),
	// Doesn't matter: compatibles from each side are exact same
	ETR_Indifferent UMETA(DisplayName = "Indifferent"),
	// // Along X
	// ETR_X_Symmetric UMETA(DisplayName = "X-Symmetric"),
	// // Along Y
	// ETR_Y_Symmetric UMETA(DisplayName = "Y-Symmetric"),
	
	ETR_MAX UMETA(DisplayName = "Default MAX")
};

UENUM()
enum class ETileSymmetry: uint8
{
	ETS_None UMETA(DisplayName = "None"),
	ETS_RtLt UMETA(DisplayName = "Right-Left"),
	ETS_FwdBwd UMETA(DisplayName = "Forward-Backward"),
	ETS_BwdLt UMETA(DisplayName = "Backward-Left"),
	ETS_FwdRt UMETA(DisplayName = "Forward-Right"),
	ETR_FourSides UMETA(DisplayName = "Four Sides Symmetry"),
	
	ETR_MAX UMETA(DisplayName = "Default MAX")
};
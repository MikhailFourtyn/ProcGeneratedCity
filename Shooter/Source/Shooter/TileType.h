#pragma once

UENUM(BlueprintType)
enum class ETileType: uint8
{
	// Initial undefined value
	ETT_Undefined UMETA(DisplayName = "Undefined"),
	// Empty tile - a valid tile which is an empty space
	ETT_Air UMETA(DisplayName = "Air"),
	
	// Roads
	ETT_Road UMETA(DisplayName = "Road"),
	ETT_Road_Crossroads UMETA(DisplayName = "Road Crossroads"),
	ETT_Road_OneLine UMETA(DisplayName = "Road OneLine"),
	ETT_Road_HalfOfWideRoad UMETA(DisplayName = "Road HalfOfWideRoad"),
	ETT_Road_Blank_Yellow_Side UMETA(DisplayName = "Road Blank_Yellow_Side"),

	// Sidewalks
	// Sidewalks work as the floor of city blocks
	// ETT_Sidewalks UMETA(DisplayName = "Sidewalks"),
	ETT_Sidewalks_Borderline UMETA(DisplayName = "Sidewalks Borderline"),
	ETT_Sidewalks_Inner UMETA(DisplayName = "Sidewalks Inner"),
	ETT_Sidewalks_Corner UMETA(DisplayName = "Sidewalks Corner"),

	// Buildings
	ETT_Building UMETA(DisplayName = "Building"),
	ETT_Building_Door_Section UMETA(DisplayName = "Building Door Section"),
	ETT_Building_Door_Corner UMETA(DisplayName = "Building Door Corner"),
	ETT_Building_Window_Section UMETA(DisplayName = "Building Window Section"),
	ETT_Building_Window_Corner UMETA(DisplayName = "Building Window Corner"),
	ETT_Building_Greeble_Cube UMETA(DisplayName = "Building Greeble Cube"),

	ETT_NoCity UMETA(Hidden),
	
	ETT_MAX UMETA(DisplayName = "Default MAX")
};

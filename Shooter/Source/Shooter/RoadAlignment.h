#pragma once

#include "CoreMinimal.h"
#include "RoadAlignment.generated.h"
UENUM()
enum class ERoadAlignment: uint8
{
	ERA_Undefined UMETA(DisplayName = "Undefined"),
	ERA_AlongX UMETA(DisplayName = "Along X"),
	ERA_AlongY UMETA(DisplayName = "Along Y"),
	
	ERA_MAX UMETA(DisplayName = "Default MAX")
};
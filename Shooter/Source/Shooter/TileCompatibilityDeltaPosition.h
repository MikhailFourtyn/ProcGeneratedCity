#pragma once

#include "CoreMinimal.h"
#include "TileCompatibilityDeltaPosition.generated.h"

UENUM()
enum class ETileCompatibilityDeltaPosition: uint8
{
	ETDP_OnForward = 0 UMETA(DisplayName = "On Forward"),
	ETDP_OnRight = 1 UMETA(DisplayName = "On Right"),
	ETDP_OnBackward = 2 UMETA(DisplayName = "On Backward"),
	ETDP_OnLeft = 3 UMETA(DisplayName = "On Left"),
	ETDP_OnTop UMETA(DisplayName = "On Top"),
	ETDP_OnBottom UMETA(DisplayName = "On Bottom")
};
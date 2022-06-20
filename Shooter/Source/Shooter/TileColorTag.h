#pragma once

UENUM(BlueprintType)
enum class ETileColorTag: uint8
{
	// Initial value
	ETCT_Indifferent UMETA(DisplayName = "Indifferent"),
	
	ETCT_01_A UMETA(DisplayName = "01_A"),
	ETCT_01_B UMETA(DisplayName = "01_B"),
	ETCT_01_C UMETA(DisplayName = "01_C"),
	ETCT_01_D UMETA(DisplayName = "01_D"),
	ETCT_01_E UMETA(DisplayName = "01_E"),
	ETCT_01_F UMETA(DisplayName = "01_F"),
	ETCT_02_A UMETA(DisplayName = "02_A"),
	ETCT_02_B UMETA(DisplayName = "02_B"),
	ETCT_02_C UMETA(DisplayName = "02_C"),
	ETCT_02_D UMETA(DisplayName = "02_D"),
	ETCT_02_E UMETA(DisplayName = "02_E"),
	ETCT_02_F UMETA(DisplayName = "02_F"),
	ETCT_03_A UMETA(DisplayName = "03_A"),
	ETCT_03_B UMETA(DisplayName = "03_B"),
	ETCT_03_C UMETA(DisplayName = "03_C"),
	ETCT_03_D UMETA(DisplayName = "03_D"),
	ETCT_03_E UMETA(DisplayName = "03_E"),
	ETCT_03_F UMETA(DisplayName = "03_F"),
	ETCT_04_A UMETA(DisplayName = "04_A"),
	ETCT_04_B UMETA(DisplayName = "04_B"),
	ETCT_04_C UMETA(DisplayName = "04_C"),
	ETCT_04_D UMETA(DisplayName = "04_D"),
	ETCT_04_E UMETA(DisplayName = "04_E"),
	ETCT_04_F UMETA(DisplayName = "04_F"),
	ETCT_01_A_Triplanar UMETA(DisplayName = "01_A_Triplanar"),
	
	ETCT_MAX UMETA(DisplayName = "Default MAX")
};

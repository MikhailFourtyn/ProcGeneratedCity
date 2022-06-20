// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ShooterGameInstance.generated.h"

/**
 * 
 */

class AGenerator;

UCLASS()
class SHOOTER_API UShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SaveGenerationParams(int smallMin, int smallMax, int largeMin, int largeMax, int wideRoadChance);

	void LoadGenerationParams(int& OutSmallMin, int &OutSmallMax, int &OutLargeMin, int &OutLargeMax,
		int &OutWideRoadChance);

private:
	UPROPERTY()
	int SmallMin;
	UPROPERTY()
	int SmallMax;
	UPROPERTY()
	int LargeMin;
	UPROPERTY()
	int LargeMax;
	UPROPERTY()
	int WideRoadChance;
};

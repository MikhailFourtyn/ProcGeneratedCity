// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameInstance.h"

void UShooterGameInstance::SaveGenerationParams(int smallMin, int smallMax, int largeMin, int largeMax,
	int wideRoadChance)
{
	SmallMin = smallMin;
	SmallMax = smallMax;
	LargeMin = largeMin;
	LargeMax = largeMax;
	WideRoadChance = wideRoadChance;
}

void UShooterGameInstance::LoadGenerationParams(int& OutSmallMin, int &OutSmallMax, int &OutLargeMin, int &OutLargeMax,
	int &OutWideRoadChance)
{
	OutSmallMin = SmallMin;
	OutSmallMax = SmallMax;
	OutLargeMin = LargeMin;
	OutLargeMax = LargeMax;
	OutWideRoadChance = WideRoadChance;
}

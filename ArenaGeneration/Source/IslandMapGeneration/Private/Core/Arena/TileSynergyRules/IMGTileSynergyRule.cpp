// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Arena/TileSynergyRules/IMGTileSynergyRule.h"

float UIMGTileSynergyRule::GetMultiplierForNeighbor(FVector TileLocation, FVector NeighborLocation,
                                                    USplineComponent* OuterBounds, USplineComponent* InnerBounds)
{
	return 1.f;
}

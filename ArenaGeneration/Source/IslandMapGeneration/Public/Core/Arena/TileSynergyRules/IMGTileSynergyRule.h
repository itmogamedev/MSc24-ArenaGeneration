// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "IMGTileSynergyRule.generated.h"

class USplineComponent;
/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, Abstract)
class ISLANDMAPGENERATION_API UIMGTileSynergyRule : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual float GetMultiplierForNeighbor(FVector TileLocation, FVector NeighborLocation,
	                                       USplineComponent* OuterBounds,
	                                       USplineComponent* InnerBounds);
};

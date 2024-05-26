// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IMGTileSynergyRule.h"
#include "IMGRadialParallelToPath.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAPGENERATION_API UIMGRadialParallelToPath : public UIMGTileSynergyRule
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Angles", meta=(ClampMin = 0.f, ClampMax = 90.f))
	float AngleBound1 = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Angles", meta=(ClampMin = 0.f, ClampMax = 90.f))
	float AngleBound2 = 90.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f), Category="Multipliers")
	float EdgeMultiplier = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f), Category="Multipliers")
	float CenterMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f), Category="Multipliers")
	float OutOfBoundsMultiplier = 0.f;

	/**
	 * @note Angle between two lines is in range of [0; 90]
	 * For shure this shit will need a rework, but it is the problem of a future me
	 */
	virtual float GetMultiplierForNeighbor(FVector TileLocation, FVector NeighborLocation,
	                                       USplineComponent* OuterBounds, USplineComponent* InnerBounds) override;

private:
	float GetAngleBetweenLines(const FVector& Start1, const FVector& End1, const FVector& Start2, const FVector& End2);
};

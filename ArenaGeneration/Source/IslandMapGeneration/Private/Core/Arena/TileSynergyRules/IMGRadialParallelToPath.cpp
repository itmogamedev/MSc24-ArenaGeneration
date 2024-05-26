// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Arena/TileSynergyRules/IMGRadialParallelToPath.h"

#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogIMGRadialToParallelPath, All, All)

float UIMGRadialParallelToPath::GetMultiplierForNeighbor(FVector TileLocation, FVector NeighborLocation,
                                                         USplineComponent* OuterBounds, USplineComponent* InnerBounds)
{
	const FVector ClosestLocationFormTileToPath = InnerBounds->FindLocationClosestToWorldLocation(
		TileLocation,
		ESplineCoordinateSpace::World);

	const FVector ClosestLocationFormNeighborToPath = InnerBounds->FindLocationClosestToWorldLocation(
		NeighborLocation,
		ESplineCoordinateSpace::World);

	float AngleBetweenLines = ClosestLocationFormNeighborToPath.Equals(ClosestLocationFormTileToPath)
		                          ? GetAngleBetweenLines(ClosestLocationFormTileToPath,
		                                                 TileLocation,
		                                                 ClosestLocationFormTileToPath,
		                                                 NeighborLocation)
		                          : GetAngleBetweenLines(TileLocation,
		                                                 NeighborLocation,
		                                                 ClosestLocationFormTileToPath,
		                                                 ClosestLocationFormNeighborToPath);

	if (!FMath::IsWithin(AngleBetweenLines, AngleBound1, AngleBound2)) { return OutOfBoundsMultiplier; }
	if (FMath::IsNearlyZero(AngleBound1))
	{
		const float Alpha = AngleBetweenLines / AngleBound2;
		return FMath::Lerp(CenterMultiplier, EdgeMultiplier, Alpha);
	}

	if (FMath::IsNearlyEqual(AngleBound2, 90.f))
	{
		const float Alpha = (AngleBetweenLines - AngleBound1) / (AngleBound2 - AngleBound1);
		return FMath::Lerp(CenterMultiplier, EdgeMultiplier, Alpha);
	}

	// Here I must complete logic, but it's enough for now
	UE_LOG(LogIMGRadialToParallelPath, Error, TEXT("YOU'VE REACHED DANGER ZONE. RETURN VALUE IS INCORRECT HERE!!!"));
	return (CenterMultiplier + EdgeMultiplier) / 2;
}

float UIMGRadialParallelToPath::GetAngleBetweenLines(const FVector& Start1, const FVector& End1, const FVector& Start2,
                                                     const FVector& End2)
{
	const FVector DirectionFromTileToNeighbor = UKismetMathLibrary::GetDirectionUnitVector(
		Start1,
		End1);

	const FVector DirectionFromClosestTilePathPointToNeighborPathPoint = UKismetMathLibrary::GetDirectionUnitVector(
		Start2,
		End2);

	float AngleBetweenVectors = FMath::RadiansToDegrees(FMath::Acos(
		FVector::DotProduct(DirectionFromTileToNeighbor, DirectionFromClosestTilePathPointToNeighborPathPoint)));

	return AngleBetweenVectors;
}

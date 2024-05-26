// Fill out your copyright notice in the Description page of Project Settings.


#include "Helpers/IMGSplineCollider.h"
#include "Components/SplineComponent.h"
#include "Helpers/IMGCustomMath.h"
#include "Voronoi/Voronoi.h"

UIMGSplineCollider::UIMGSplineCollider()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetClosedLoop(true);
	Duration = 100.f;
}

bool UIMGSplineCollider::IsPointInsideSplineXY(FVector Point) const
{
	int32 IntersectionsCount = 0;
	for (int32 PointIndex = 0; PointIndex < GetNumberOfSplinePoints() - 1; ++PointIndex)
	{
		const bool IntersectsSegment = UIMGCustomMath::IsLinesIntersects(
			Point,
			FVector(Point.X + IsInsideSplinePointTraceLength, Point.Y, Point.Z),
			GetLocationAtSplinePoint(PointIndex, ESplineCoordinateSpace::World),
			GetLocationAtSplinePoint(PointIndex + 1, ESplineCoordinateSpace::World)
		);

		if (IntersectsSegment) { ++IntersectionsCount; }
	}

	const bool IntersectsSegment = UIMGCustomMath::IsLinesIntersects(
		Point,
		FVector(Point.X + IsInsideSplinePointTraceLength, Point.Y, Point.Z),
		GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World),
		GetLocationAtSplinePoint(GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World)
	);
	if (IntersectsSegment) { ++IntersectionsCount; }

	if (IntersectionsCount == 0) { return false; }
	return IntersectionsCount % 2 == 1;
}

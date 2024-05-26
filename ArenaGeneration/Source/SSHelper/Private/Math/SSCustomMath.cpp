// Fill out your copyright notice in the Description page of Project Settings.

#include "Math/SSCustomMath.h"

#include "Components/SplineComponent.h"

int32 USSCustomMath::GetRandomWeightedIndex(TArray<float>& Weights)
{
	float TotalWeight = 0.f;
	for (float Weight : Weights) { TotalWeight += Weight; }

	float RandWeight = FMath::RandRange(0.f, TotalWeight);
	float CurrentWeight = 0.f;
	for (int32 i = 0; i < Weights.Num(); ++i)
	{
		CurrentWeight += Weights[i];
		if (RandWeight < CurrentWeight || FMath::IsNearlyEqual(CurrentWeight, RandWeight)) { return i; }
	}

	return Weights.Num() - 1;
}

bool USSCustomMath::IsLinesIntersects(FVector P1, FVector Q1, FVector P2, FVector Q2)
{
	const ELineOrientations O1 = CheckLinesOrientation(P1, Q1, P2);
	const ELineOrientations O2 = CheckLinesOrientation(P1, Q1, Q2);
	const ELineOrientations O3 = CheckLinesOrientation(P2, Q2, P1);
	const ELineOrientations O4 = CheckLinesOrientation(P2, Q2, Q1);

	if (O1 != O2 && O3 != O4) { return true; }

	if (O1 == ELineOrientations::ELO_Collinear && IsLineOnSegment(P1, P2, Q1)) { return true; }
	if (O2 == ELineOrientations::ELO_Collinear && IsLineOnSegment(P1, Q2, Q1)) { return true; }
	if (O3 == ELineOrientations::ELO_Collinear && IsLineOnSegment(P2, P2, Q2)) { return true; }
	if (O4 == ELineOrientations::ELO_Collinear && IsLineOnSegment(P2, Q1, Q2)) { return true; }

	return false;
}

void USSCustomMath::SortClockwise(FVector CenterPoint, TArray<FVector>& Points)
{
	Points.Sort([CenterPoint](const FVector& A, const FVector& B)
	{
		if (A.X - CenterPoint.X >= 0 && B.X - CenterPoint.X < 0) { return true; }
		if (A.X - CenterPoint.X < 0 && B.X - CenterPoint.X >= 0) { return false; }
		if (A.X - CenterPoint.X == 0 && B.X - CenterPoint.X == 0)
		{
			if (A.Y - CenterPoint.Y >= 0 || B.Y - CenterPoint.Y >= 0) { return A.Y > B.Y; }
			return B.Y > A.Y;
		}

		const float det = (A.X - CenterPoint.X) * (B.Y - CenterPoint.Y) - (B.X - CenterPoint.X) * (A.Y - CenterPoint.Y);
		if (det < 0) { return true; }
		if (det > 0) { return false; }

		const float d1 = (A.X - CenterPoint.X) * (A.X - CenterPoint.X) + (A.Y - CenterPoint.Y) * (A.Y - CenterPoint.Y);
		const float d2 = (B.X - CenterPoint.X) * (B.X - CenterPoint.X) + (B.Y - CenterPoint.Y) * (B.Y - CenterPoint.Y);
		return d1 > d2;
	});
}

float USSCustomMath::GetSplineTimeAtLocationClosestToPoint(const FVector& Point,
                                                           const USplineComponent* SplineComponent)
{
	const float InputKey = SplineComponent->FindInputKeyClosestToWorldLocation(Point);
	const float Distance = SplineComponent->GetDistanceAlongSplineAtSplineInputKey(InputKey);
	return SplineComponent->GetTimeAtDistanceAlongSpline(Distance);
}

float USSCustomMath::GetSplineLengthAtLocationClosestToPointClamped(const FVector& Point,
                                                                    const USplineComponent* SplineComponent)
{
	float TimeAtPoint = GetSplineTimeAtLocationClosestToPoint(Point, SplineComponent);
	return TimeAtPoint / SplineComponent->Duration;
}

float USSCustomMath::RoundFloatTo3Digit(float A)
{
	A *= 1000.f;
	return FMath::RoundToInt32(A) * 0.001;
}

void USSCustomMath::RoundVectorTo3Digit(FVector& Vector)
{
	Vector.X = RoundFloatTo3Digit(Vector.X);
	Vector.Y = RoundFloatTo3Digit(Vector.Y);
	Vector.Z = RoundFloatTo3Digit(Vector.Z);
}

ELineOrientations USSCustomMath::CheckLinesOrientation(FVector P, FVector Q, FVector R)
{
	float val = (Q.Y - P.Y) * (R.X - Q.X) - (Q.X - P.X) * (R.Y - Q.Y);
	if (FMath::IsNearlyEqual(val, 0.f)) { return ELineOrientations::ELO_Collinear; }

	return val > 0.f
		       ? ELineOrientations::ELO_Clockwise
		       : ELineOrientations::ELO_CounterClockwise;
}

bool USSCustomMath::IsLineOnSegment(FVector P, FVector Q, FVector R)
{
	return (Q.X <= FMath::Max(P.X, R.X) && Q.X >= FMath::Min(P.X, R.X) &&
		Q.Y <= FMath::Max(P.Y, R.Y) && Q.Y >= FMath::Min(P.Y, R.Y));
}

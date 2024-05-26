// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Arena/GlobalLocationRules/IMGGlobalLocationRule.h"

#include "Core/IMGTypes.h"
#include "Helpers/IMGCustomMath.h"


float UIMGGlobalLocationRule::GetMultiplierForPoint(FVector Point, USplineComponent* OuterBounds,
                                                    USplineComponent* InnerBounds, EIMGArenaType Type)
{
	check(OuterBounds);
	check(InnerBounds);
	if (Rules.IsEmpty()) { return 1.f; }

	float TotalMultiplier = 0.f;
	for (auto Rule : Rules)
	{
		float Multiplier = 0.f;
		switch (Rule.GradientType)
		{
			case EIMGGlobalLocationRulesGradient::EGLRG_ParallelLiner:
				Multiplier = CalculateMultiplierForLinerGradient(Point, OuterBounds, InnerBounds, Type, Rule);
				break;
			case EIMGGlobalLocationRulesGradient::EGLRG_AlongDistanceBetweenBounds:
				Multiplier = CalculateMultiplierForGradientAlongDistanceBetweenBounds(
					Point,
					OuterBounds,
					InnerBounds,
					Rule);
				break;
		}

		switch (Rule.GradientBehavior)
		{
			case EIMGGradientBehavior::EGB_Additive:
				if (FMath::IsNearlyEqual(Multiplier, 0.f)) { return 0.f; }
				TotalMultiplier += Multiplier;
				break;
			case EIMGGradientBehavior::EGB_Subtractive:
				TotalMultiplier -= Multiplier;
				break;
		}
	}

	return FMath::Clamp(TotalMultiplier, 0.f, TNumericLimits<float>::Max());
}

float UIMGGlobalLocationRule::CalculateMultiplierForLinerGradient(FVector Point, USplineComponent* OuterBounds,
                                                                  USplineComponent* PathSpline, EIMGArenaType Type,
                                                                  FIMGLocationRuleGradient& Rule)
{
	// Arena PathSpline length is almost equal 0, and zones should be rounded, so we use another function  
	if (Type == EIMGArenaType::EAT_Arena)
	{
		return CalculateMultiplierForGradientAlongDistanceBetweenBounds(Point, OuterBounds, PathSpline, Rule);
	}

	const float LengthClosestToPoint =
		UIMGCustomMath::GetSplineLengthAtLocationClosestToPointClamped(Point, PathSpline);
	if (!FMath::IsWithin(LengthClosestToPoint, Rule.Bound1, Rule.Bound2)) { return Rule.OutOfBoundsMultiplier; }

	const float CenterPoint = (Rule.Bound1 + Rule.Bound2) / 2;
	if (FMath::Abs(Rule.Bound1 - LengthClosestToPoint) < FMath::Abs(Rule.Bound2 - LengthClosestToPoint))
	{
		const float Alpha = (LengthClosestToPoint - Rule.Bound1) / (CenterPoint - Rule.Bound1);
		return FMath::Lerp(Rule.Bound1Multiplier, Rule.CenterMultiplier, Alpha);
	}
	const float Alpha = (LengthClosestToPoint - CenterPoint) / (Rule.Bound2 - CenterPoint);
	return FMath::Lerp(Rule.CenterMultiplier, Rule.Bound2Multiplier, Alpha);
}

float UIMGGlobalLocationRule::CalculateMultiplierForGradientAlongDistanceBetweenBounds(
	FVector Point, USplineComponent* OuterBounds, USplineComponent* InnerBounds, FIMGLocationRuleGradient& Rule)
{
	const FVector ClosestPathPoint = InnerBounds->FindLocationClosestToWorldLocation(
		Point,
		ESplineCoordinateSpace::World);
	const FVector ClosestOuterPoint = OuterBounds->FindLocationClosestToWorldLocation(
		Point,
		ESplineCoordinateSpace::World);
	const float DistanceBetweenPathAndOuterSpline = FVector::Distance(ClosestOuterPoint, ClosestPathPoint);

	const float DistanceToPathSpline = FVector::Distance(Point, ClosestPathPoint);
	const float LengthAtPoint = DistanceToPathSpline / DistanceBetweenPathAndOuterSpline;

	if (!FMath::IsWithin(LengthAtPoint, Rule.Bound1, Rule.Bound2)) { return Rule.OutOfBoundsMultiplier; }

	const float CenterPoint = (Rule.Bound1 + Rule.Bound2) / 2;
	if (FMath::Abs(Rule.Bound1 - LengthAtPoint) < FMath::Abs(Rule.Bound2 - LengthAtPoint))
	{
		const float Alpha = (LengthAtPoint - Rule.Bound1) / (CenterPoint - Rule.Bound1);
		return FMath::Lerp(Rule.Bound1Multiplier, Rule.CenterMultiplier, Alpha);
	}
	const float Alpha = (LengthAtPoint - CenterPoint) / (Rule.Bound2 - CenterPoint);
	return FMath::Lerp(Rule.CenterMultiplier, Rule.Bound2Multiplier, Alpha);
}

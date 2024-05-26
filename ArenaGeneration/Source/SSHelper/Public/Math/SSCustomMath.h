// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Math/SSCustomMathTypes.h"
#include "SSCustomMath.generated.h"

class USplineComponent;
/**
 * 
 */
UCLASS()
class SSHELPER_API USSCustomMath : public UObject
{
	GENERATED_BODY()

public:
	static int32 GetRandomWeightedIndex(TArray<float>& Weights);
	static bool IsLinesIntersects(FVector P1, FVector Q1, FVector P2, FVector Q2);
	static void SortClockwise(FVector CenterPoint, TArray<FVector>& Points);

	static float GetSplineTimeAtLocationClosestToPoint(const FVector& Point, const USplineComponent* SplineComponent);

	/**
	 * @details Returns value between 0 and 1
	 */
	static float GetSplineLengthAtLocationClosestToPointClamped(const FVector& Point,
	                                                            const USplineComponent* SplineComponent);
	static float RoundFloatTo3Digit(float A);
	static void RoundVectorTo3Digit(FVector& Vector);

	template <class T>
	static T GetRandomWeightedClassFromTupleArray(TArray<TTuple<T, float>> TupleArray);

	template <class T>
	static T GetBiggestWeightClassFromTupleArray(TArray<TTuple<T, float>> TupleArray);

private:
	UFUNCTION(BlueprintCallable)
	static ELineOrientations CheckLinesOrientation(FVector P, FVector Q, FVector R);

	UFUNCTION(BlueprintCallable)
	static bool IsLineOnSegment(FVector P, FVector Q, FVector R);
};

template <class T>
T USSCustomMath::GetRandomWeightedClassFromTupleArray(TArray<TTuple<T, float>> TupleArray)
{
	float TotalWeight = 0.f;
	for (auto Tuple : TupleArray) { TotalWeight += Tuple.Value; }

	float RandWeight = FMath::RandRange(0.f, TotalWeight);
	float CurrentWeight = 0.f;
	for (int32 i = 0; i < TupleArray.Num(); ++i)
	{
		CurrentWeight += TupleArray[i].Value;
		if (RandWeight < CurrentWeight || FMath::IsNearlyEqual(CurrentWeight, RandWeight)) { return TupleArray[i].Key; }
	}

	return TupleArray[TupleArray.Num() - 1].Key;
}

// If there are few classes with equal weights -> return random from them
template <class T>
T USSCustomMath::GetBiggestWeightClassFromTupleArray(TArray<TTuple<T, float>> TupleArray)
{
	TArray BiggestWeightsArray{TupleArray[0]};
	for (const auto Tuple : TupleArray)
	{
		if (Tuple.Value > BiggestWeightsArray[0].Value)
		{
			BiggestWeightsArray.Empty();
			BiggestWeightsArray.Add(Tuple);
		}
		else if (FMath::IsNearlyEqual(Tuple.Value, BiggestWeightsArray[0].Value)) { BiggestWeightsArray.Add(Tuple); }
	}

	if (BiggestWeightsArray.Num() == 1) { return BiggestWeightsArray[0].Key; }

	int32 RandomIndex = FMath::RandRange(0, BiggestWeightsArray.Num() - 1);
	return BiggestWeightsArray[RandomIndex].Key;
}

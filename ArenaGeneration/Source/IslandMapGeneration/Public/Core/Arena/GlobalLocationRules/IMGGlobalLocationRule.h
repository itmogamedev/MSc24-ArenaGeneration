// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/IMGTypes.h"
#include "UObject/Object.h"
#include "IMGGlobalLocationRule.generated.h"


class USplineComponent;

UENUM(BlueprintType)
enum class EIMGGlobalLocationRulesGradient : uint8
{
	EGLRG_ParallelLiner UMETA(DisplayName = "Parallel Linear"),
	EGLRG_AlongDistanceBetweenBounds UMETA(DisplayName = "Along Distance Between Bounds"),

	EGLRG_MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EIMGGradientBehavior : uint8
{
	EGB_Additive UMETA(DisplayName = "Additive"),
	EGB_Subtractive UMETA(DisplayName = "Subtractive"),

	EGB_MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct ISLANDMAPGENERATION_API FIMGLocationRuleGradient
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gradient")
	EIMGGlobalLocationRulesGradient GradientType = EIMGGlobalLocationRulesGradient::EGLRG_ParallelLiner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gradient")
	EIMGGradientBehavior GradientBehavior = EIMGGradientBehavior::EGB_Additive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, ClampMax = 1.f), Category="Bounds")
	float Bound1 = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, ClampMax = 1.f), Category="Bounds")
	float Bound2 = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f), Category="Multipliers")
	float Bound1Multiplier = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f), Category="Multipliers")
	float Bound2Multiplier = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f), Category="Multipliers")
	float CenterMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f), Category="Multipliers")
	float OutOfBoundsMultiplier = 0.f;
};

UCLASS(Blueprintable, EditInlineNew)
class ISLANDMAPGENERATION_API UIMGGlobalLocationRule : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FIMGLocationRuleGradient> Rules{};

	UFUNCTION(BlueprintCallable)
	float GetMultiplierForPoint(FVector Point, USplineComponent* OuterBounds, USplineComponent* InnerBounds,
	                            EIMGArenaType Type);

private:
	// The problem is linear gradient can not be calculated
	UFUNCTION(BlueprintCallable)
	float CalculateMultiplierForLinerGradient(FVector Point, USplineComponent* OuterBounds,
	                                          USplineComponent* PathSpline, EIMGArenaType Type,
	                                          FIMGLocationRuleGradient& Rule);

	UFUNCTION(BlueprintCallable)
	float CalculateMultiplierForGradientAlongDistanceBetweenBounds(FVector Point, USplineComponent* OuterBounds,
	                                                               USplineComponent* InnerBounds,
	                                                               FIMGLocationRuleGradient& Rule);
};

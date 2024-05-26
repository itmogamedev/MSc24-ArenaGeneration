// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "IMGSplineCollider.generated.h"

class UBoxComponent;
class USplineComponent;

UCLASS()
class ISLANDMAPGENERATION_API UIMGSplineCollider : public USplineComponent
{
	GENERATED_BODY()

public:
	UIMGSplineCollider();

	UFUNCTION(BlueprintCallable)
	bool IsPointInsideSplineXY(FVector Point) const;

private:
	UPROPERTY(EditAnywhere)
	float IsInsideSplinePointTraceLength = 10000000.f;
};

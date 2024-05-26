// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IMGTypes.h"
#include "DataAssets/IMGMesoPatternBuildRules.h"
#include "IMGIsland.generated.h"

class UStaticMeshComponent;
class USplineComponent;
class USphereComponent;

UCLASS()
class ISLANDMAPGENERATION_API AIMGIsland : public AActor
{
	GENERATED_BODY()

public:
	AIMGIsland();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USphereComponent* VicinityCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EIMGOverlapType CanBeOverlapped;

	UFUNCTION(BlueprintNativeEvent)
	void GenerationFinished();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* IslandBase;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EIMGIslandType IslandType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EIMGIslandSize Size;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USplineComponent* InnerBoundsSpline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USplineComponent* OuterBoundsSpline;

	UFUNCTION()
	virtual void GenerateLandscape();
	
};

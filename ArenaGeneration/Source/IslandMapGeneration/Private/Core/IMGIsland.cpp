// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/IMGIsland.h"
#include "Components/SplineComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogIMGIsland, All, All)

AIMGIsland::AIMGIsland()
{
	PrimaryActorTick.bCanEverTick = false;
	
	InnerBoundsSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Inner bounds"));
	InnerBoundsSpline->SetupAttachment(GetRootComponent());
	InnerBoundsSpline->Duration = 1000.f;

	OuterBoundsSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Outer bounds"));
	OuterBoundsSpline->SetupAttachment(GetRootComponent());
	OuterBoundsSpline->SetClosedLoop(true);
	OuterBoundsSpline->Duration = 1000.f;
}

void AIMGIsland::BeginPlay()
{
	Super::BeginPlay();
}

void AIMGIsland::GenerateLandscape()
{
}


void AIMGIsland::GenerationFinished_Implementation()
{
}

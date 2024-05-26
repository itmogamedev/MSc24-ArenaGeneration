// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/IMGIsland.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogIMGIsland, All, All)

AIMGIsland::AIMGIsland()
{
	PrimaryActorTick.bCanEverTick = false;

	IslandBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Island Base"));
	SetRootComponent(IslandBase);

	InnerBoundsSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Inner bounds"));
	InnerBoundsSpline->SetupAttachment(GetRootComponent());
	InnerBoundsSpline->Duration = 1000.f;

	OuterBoundsSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Outer bounds"));
	OuterBoundsSpline->SetupAttachment(GetRootComponent());
	OuterBoundsSpline->SetClosedLoop(true);
	OuterBoundsSpline->Duration = 1000.f;

	VicinityCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Island vicinity sphere collision"));
	VicinityCollision->SetupAttachment(GetRootComponent());
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

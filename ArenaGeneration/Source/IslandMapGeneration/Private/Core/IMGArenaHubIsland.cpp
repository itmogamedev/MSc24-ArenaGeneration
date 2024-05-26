// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/IMGArenaHubIsland.h"


AIMGArenaHubIsland::AIMGArenaHubIsland()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AIMGArenaHubIsland::BeginPlay()
{
	Super::BeginPlay();

	GenerationFinished();
}



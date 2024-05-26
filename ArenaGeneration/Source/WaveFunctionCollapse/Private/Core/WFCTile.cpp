// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/WFCTile.h"


AWFCTile::AWFCTile()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tile mesh"));
	SetRootComponent(Mesh);
}

void AWFCTile::BeginPlay() { Super::BeginPlay(); }

// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Arena/IMGArenaGenerator.h"

#include "Core/IMGArenaIsland.h"
#include "Core/Arena/IMGArenaGenerationHelper.h"
#include "DataAssets/IMGEnemyMapArenaGenerationDB.h"
#include "Kismet/GameplayStatics.h"


AIMGArenaGenerator::AIMGArenaGenerator() { PrimaryActorTick.bCanEverTick = false; }

void AIMGArenaGenerator::GenerateArena()
{
	RemoveCurrentArena();
	FlushPersistentDebugLines(GetWorld());
	FlushDebugStrings(GetWorld());

	if (RandomDependsOnEnemies) { DependsOnEnemies = !DependsOnEnemies; }
	// Choose Arena Type
	EIMGArenaType ArenaType = DependsOnEnemies
		                          ? FIMGArenaGenerationHelper::ChooseMacroPattern(
			                          EnemiesToSpawn,
			                          EnemyMapArena->Data)
		                          : static_cast<EIMGArenaType>(FMath::RandRange(0, 1));

	// Pick random Arena class
	TArray<TSubclassOf<AIMGArenaIsland>> ChosenPool = ArenasPool.Find(ArenaType)->Array;

	if (ChosenPool.Num() == 0) { return; }

	const int32 RandIndex = FMath::RandRange(0, ChosenPool.Num() - 1);
	const TSubclassOf<AIMGArenaIsland> ChosenClass = ChosenPool[RandIndex];

	// Spawn Arena
	CurrentArenaIsland = GetWorld()->SpawnActor<AIMGArenaIsland>(ChosenClass,
	                                                             GetActorLocation(),
	                                                             GetActorRotation());
	CurrentArenaIsland->DependsOnEnemies = DependsOnEnemies;

	CurrentArenaIsland->GenerateArenaGeometry(EnemiesToSpawn, EnemyMapArena);
}

void AIMGArenaGenerator::RemoveCurrentArena()
{
	if (CurrentArenaIsland) { CurrentArenaIsland->Destroy(); }
}

void AIMGArenaGenerator::BeginPlay() { Super::BeginPlay(); }

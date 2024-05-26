// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/IMGTypes.h"
#include "UObject/Object.h"
#include "Example/ExampleEnemy.h"

class AGHBaseEnemy;

class ISLANDMAPGENERATION_API FIMGArenaGenerationHelper
{
public:
	static EIMGArenaType ChooseMacroPattern(EnemiesCount EnemiesToSpawn,
	                                        EnemiesGenerationData Data);

	static void GetArenaTilesMaxNum(TMap<EIMGArenaTileType, int32>& TilesMaxNum,
	                                int32 TotalTilesNum,
	                                const EnemiesCount& EnemiesToSpawn,
	                                const EnemiesGenerationData& Data,
	                                const TMap<EIMGEnemyMovementType, FIMGTileTypeToMultiplier>&
	                                MovementTypeToArenaTypeImpact);

	static float GetTotalEnemiesSetEfficiency(const EnemiesCount& EnemiesToSpawn,
	                                          const EnemiesGenerationData& Data);
};

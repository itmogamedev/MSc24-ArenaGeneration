// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/IMGTypes.h"
#include "Interfaces/Enemy.h"
#include "UObject/Object.h"

class AGHBaseEnemy;

class ISLANDMAPGENERATION_API FIMGArenaGenerationHelper
{
public:
	static EIMGArenaType ChooseMacroPattern(TMap<TSubclassOf<UEnemy>, int32> EnemiesToSpawn,
	                                        TMap<TSubclassOf<UEnemy>, FIMGEnemyGenerationData> Data);

	static void GetArenaTilesMaxNum(TMap<EIMGArenaTileType, int32>& TilesMaxNum,
	                                int32 TotalTilesNum,
	                                const TMap<TSubclassOf<UEnemy>, int32>& EnemiesToSpawn,
	                                const TMap<TSubclassOf<UEnemy>, FIMGEnemyGenerationData>& Data,
	                                const TMap<EIMGEnemyMovementType, FIMGTileTypeToMultiplier>&
	                                MovementTypeToArenaTypeImpact);

	static float GetTotalEnemiesSetEfficiency(const TMap<TSubclassOf<UEnemy>, int32>& EnemiesToSpawn,
	                                          const TMap<TSubclassOf<UEnemy>, FIMGEnemyGenerationData>& Data);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/IMGTypes.h"
#include "UObject/Object.h"
#include "Voronoi/Voronoi.h"
#include "DataAssets/IMGMesoPatternBuildRules.h"

class UIMGEnemyMapArenaGenerationDB;
class AIMGArenaIsland;
class USplineComponent;
class AGHBaseEnemy;
class UBoxComponent;
class UIMGSplineCollider;
class UIMGMesoPatternBuildRules;

class ISLANDMAPGENERATION_API FIMGArenaFloorGenerator
{
public:
	friend AIMGArenaIsland;

	FIMGArenaFloorGenerator();

	AIMGArenaIsland* ParentArena;
	EIMGArenaType Type = EIMGArenaType::EAT_Arena;
	UIMGEnemyMapArenaGenerationDB* EnemyMapArena;

	bool DependsOnEnemies = true;

	/**
	 * @param NumOfInitialCells		Stands for all cells that are going to be generated.
	 *								Note that practically number of cells will be less because some of them will be erased by arena bounds
	 * @param BoundingBox			Points will be generated inside of this box
	 * @param ArenaBoundsCollider
	 */
	FIMGArenaFloorGenerator(int32 NumOfInitialCells, UBoxComponent* BoundingBox,
	                        UIMGSplineCollider* ArenaBoundsCollider, UIMGMesoPatternBuildRules* Rules);

	void Initialize(int32 NumOfInitialCells, UBoxComponent* BoundingBox, UIMGSplineCollider* ArenaBoundsCollider,
	                UIMGMesoPatternBuildRules* Rules);

	void SplitIntoMesoPatterns(EnemiesCount EnemiesCount, USplineComponent* PathSpline);
	void GetPerTileProcMeshBuildData(TArray<FIMGExtrudeData>& ExtrudeData);

	void DebugTileTypes(const UWorld* World);
	void DebugClusters(const UWorld* World);

private:
	FVoronoiDiagram VoronoiDiagram;
	TArray<FIMGCellData*> Cells{};
	UIMGSplineCollider* ArenaBounds;
	UIMGMesoPatternBuildRules* MesoPatternCollapseRules;

	TArray<FIMGCellData*> CellsToTraverse;
	TArray<FIMGCellData*> CellsToJoinIntoCluster;

	EnemiesCount EnemiesCountToSpawn;
	TMap<EIMGArenaTileType, int32> TilesMaxNum;
	TMap<EIMGArenaTileType, int32> TilesSpawned;

	TArray<FIMGClusterData*> ClustersData;
	FIMGClusterData* BufferCluster;
	TArray<FIMGCellData*> BufferCells;

	void CheckAndRemoveOverflowedTileTypesFromEntropy(EIMGArenaTileType TileType);

	FIMGCellData* GetCellClosestToPoint(const FVector& Point);
	FIMGCellData* GetCellWithLeastEntropy();

	void CollapseMesoPatterns(FIMGCellData* Cell);
	void CollapseToType(FIMGCellData* Cell, EIMGArenaTileType TileType);
	void Propagate(FIMGCellData* Cell);

	void ApplyLocationModifiers(FIMGCellData* Cell);
	void ApplySynergyModifiers(FIMGCellData* Cell);

	void JoinIntoClusters(FIMGCellData* StartCell);
	void JoinAllTypedNeighboursIntoCluster(FIMGCellData* Cell);

	void ApplyHeightToClusters();

public:
	FORCEINLINE void SetArenaBounds(UIMGSplineCollider* Collider) { ArenaBounds = Collider; }
	FORCEINLINE TArray<FIMGClusterData*> GetClustersData() { return ClustersData; }
};

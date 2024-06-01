// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/IMGArenaIsland.h"

#include "AI/NavigationSystemBase.h"
#include "Components/BoxComponent.h"
#include "Components/DynamicMeshComponent.h"
#include "Helpers/IMGSplineCollider.h"
#include "Containers/ArrayView.h"
#include "Core/WFCActor.h"
#include "DataAssets/IMGTileSets.h"
#include "Math/Box.h"
#include "DynamicMesh/DynamicMesh3.h"

DEFINE_LOG_CATEGORY_STATIC(LogIMGArenaIsland, All, All)

AIMGArenaIsland::AIMGArenaIsland()
{
	PrimaryActorTick.bCanEverTick = false;

	ArenaPointsScatterBounds = CreateDefaultSubobject<UBoxComponent>("ArenaPointsScatterBounds");
	ArenaPointsScatterBounds->SetupAttachment(GetRootComponent());

	ArenaFloorBounds = CreateDefaultSubobject<UIMGSplineCollider>("SplineCollider");
	ArenaFloorBounds->SetupAttachment(ArenaPointsScatterBounds);

	FloorDynamicMesh = CreateDefaultSubobject<UDynamicMeshComponent>("FloorDynamicMesh");
	FloorDynamicMesh->SetupAttachment(ArenaPointsScatterBounds);
}

void AIMGArenaIsland::GenerateArenaGeometry(EnemiesCount EnemiesToSpawn,
                                            UIMGEnemyMapArenaGenerationDB* EnemyMapArena)
{
	GenerateFloor(EnemiesToSpawn, EnemyMapArena);

	GenerationFinished();
}

void AIMGArenaIsland::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (FIMGClusterData* Cluster : ArenaFloorGenerator.ClustersData)
	{
		if (Cluster->WFCActor) { Cluster->WFCActor->Destroy(); }
	}

	Super::EndPlay(EndPlayReason);
}

void AIMGArenaIsland::GenerateFloor(EnemiesCount EnemiesToSpawn,
                                    UIMGEnemyMapArenaGenerationDB* EnemyMapArena)
{
	ArenaFloorGenerator = FIMGArenaFloorGenerator(ScatterPointsNum,
	                                              ArenaPointsScatterBounds,
	                                              ArenaFloorBounds,
	                                              CollapseRules);
	ArenaFloorGenerator.DependsOnEnemies = DependsOnEnemies;
	ArenaFloorGenerator.ParentArena = this;
	ArenaFloorGenerator.EnemyMapArena = EnemyMapArena;
	ArenaFloorGenerator.SplitIntoMesoPatterns(EnemiesToSpawn, InnerBoundsSpline);
	ArenaFloorGenerator.DebugTileTypes(GetWorld());
	// ArenaFloorGenerator.DebugClusters(GetWorld());

	ArenaFloorGenerator.GetPerTileProcMeshBuildData(ExtrudeData);

	for (FIMGCellData* Cell : ArenaFloorGenerator.Cells)
	{
		switch (Cell->Type)
		{
			case EIMGArenaTileType::EATT_SniperPosition:
			case EIMGArenaTileType::EATT_Gallery:
			case EIMGArenaTileType::EATT_ChokePoint:
			case EIMGArenaTileType::EATT_Battle:
			case EIMGArenaTileType::EATT_AlternativePath:
				SpawnPoints.Add(Cell->Site + FVector(0.f, 0.f, 1000.f));
				break;
		}
	}
	// TODO: Figure out what is really needed =============================================
	// FloorDynamicMesh->SetRelativeLocation(-GetActorLocation());
	FloorDynamicMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FloorDynamicMesh->SetDeferredCollisionUpdatesEnabled(true);
	FloorDynamicMesh->bEnableComplexCollision = true;
	FloorDynamicMesh->EnableComplexAsSimpleCollision();
	// =====================================================================================

	FNavigationSystem::UpdateComponentData(*FloorDynamicMesh);

	for (FIMGClusterData* Cluster : ArenaFloorGenerator.ClustersData)
	{
		switch (Cluster->Type)
		{
			case EIMGArenaTileType::EATT_Empty:
			case EIMGArenaTileType::EATT_Start:
				PlayerSpawnPoint = Cluster->Cells[0]->Site + FVector(0.f, 0.f, 200.f);
				EndLevel = Cluster->Cells[0]->Site;
				continue;
			case EIMGArenaTileType::EATT_End:
				EndLevel = Cluster->Cells[0]->Site;
				continue;
			case EIMGArenaTileType::EATT_Blocked:
			case EIMGArenaTileType::EATT_MAX:
				continue;
			default:
				FVector OutRectCenter;
				FRotator OutRectRotation;
				float OutSideLengthX;
				float OutSideLengthY;

				Cluster->GetBoxAroundCluster(GetWorld(),
				                             OutRectCenter,
				                             OutRectRotation,
				                             OutSideLengthX,
				                             OutSideLengthY,
				                             false);
				OutRectCenter.Z = Cluster->Cells[0]->Site.Z;
				Cluster->WFCActor = GetWorld()->SpawnActor<AWFCActor>(WFCActorClass,
				                                                      OutRectCenter,
				                                                      OutRectRotation);

				UWFCTileSet* TileSet = *TileSets->TileSets.Find(Cluster->Type);
				Cluster->WFCActor->SetupGrid(TileSet,
				                             FVector(OutSideLengthX, OutSideLengthY, 10.f),
				                             FVector::One(),
				                             [Cluster](const FVector& Point)
				                             {
					                             return Cluster->IsPointInsideCluster(Point);
				                             });
				Cluster->WFCActor->Generate();
				break;
		}
	}
}

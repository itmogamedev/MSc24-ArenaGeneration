// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Arena/IMGArenaFloorGenerator.h"
#include "Components/BoxComponent.h"
#include "Core/IMGArenaIsland.h"
#include "Core/Arena/IMGArenaGenerationHelper.h"
#include "Helpers/IMGCustomMath.h"
#include "Helpers/IMGSplineCollider.h"
#include "Kismet/KismetMathLibrary.h"
#include "Core/Arena/GlobalLocationRules/IMGGlobalLocationRule.h"
#include "DataAssets/IMGEnemyMapArenaGenerationDB.h"
#include "DynamicMesh/DynamicMesh3.h"

DEFINE_LOG_CATEGORY_STATIC(LogIMGTilesdVoronoiArenaFloor, All, All)

FIMGArenaFloorGenerator::FIMGArenaFloorGenerator()
{
}

FIMGArenaFloorGenerator::FIMGArenaFloorGenerator(int32 NumOfInitialCells, UBoxComponent* BoundingBox,
                                                 UIMGSplineCollider* ArenaBoundsCollider,
                                                 UIMGMesoPatternBuildRules* Rules)
{
	Initialize(NumOfInitialCells, BoundingBox, ArenaBoundsCollider, Rules);
}

void FIMGArenaFloorGenerator::Initialize(int32 NumOfInitialCells, UBoxComponent* BoundingBox,
                                         UIMGSplineCollider* ArenaBoundsCollider, UIMGMesoPatternBuildRules* Rules)
{
	check(Rules != nullptr);
	check(BoundingBox != nullptr);
	check(ArenaBoundsCollider != nullptr);

	MesoPatternCollapseRules = Rules;
	SetArenaBounds(ArenaBoundsCollider);

	TArray<FVector> Sites{};
	// Picking random points inside box
	for (int32 i = 0; i < NumOfInitialCells; ++i)
	{
		FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(
			BoundingBox->GetComponentLocation(),
			BoundingBox->GetScaledBoxExtent());
		RandomPoint.Z = BoundingBox->GetComponentLocation().Z;
		Sites.Add(RandomPoint);
	}

	const TArrayView<const FVector> SitesViewArray(Sites);

	const FBox BoundingBoxExtents = FBox::BuildAABB(BoundingBox->GetComponentLocation(),
	                                                BoundingBox->GetScaledBoxExtent());
	// Creating Voronoi diagram
	VoronoiDiagram = {SitesViewArray, BoundingBoxExtents, 0};
	TArray<FVoronoiCellInfo> CellsInfo;
	VoronoiDiagram.ComputeAllCells(CellsInfo);


	// Deep copy
	for (auto& CellInfo : CellsInfo) { Cells.Add(new FIMGCellData{CellInfo}); }

	for (int32 CellIndex = 0; CellIndex < Cells.Num(); ++CellIndex)
	{
		Cells[CellIndex]->Site = Sites[CellIndex];
		for (auto NeighborIndex : Cells[CellIndex]->Neighbors)
		{
			if (NeighborIndex >= 0 && ArenaBounds->IsPointInsideSplineXY(Sites[NeighborIndex]))
			{
				FIMGCellData* CellData = Cells[NeighborIndex];
				Cells[CellIndex]->NeighborsPtrs.Add(CellData);
			}
		}
	}

	// Removing invalid indices
	Cells.RemoveAll([&](const FIMGCellData* A) { return !ArenaBounds->IsPointInsideSplineXY(A->Site); });

	// Remove duplicated vertices
	for (FIMGCellData* Cell : Cells)
	{
		TArray<FVector> UniqueSiteVertices;
		for (FVector& SiteVertex : Cell->Vertices)
		{
			const FVector* FoundVertex = UniqueSiteVertices.FindByPredicate([&](const FVector& A)
			{
				return A.Equals(SiteVertex);
			});

			if (FoundVertex) { continue; }
			UniqueSiteVertices.Add(SiteVertex);
		}
		Cell->Vertices = UniqueSiteVertices;
	}
}

void FIMGArenaFloorGenerator::SplitIntoMesoPatterns(TMap<TSubclassOf<UEnemy>, int32> EnemiesCount,
                                                    USplineComponent* PathSpline)
{
	CellsToTraverse = Cells;
	CellsToJoinIntoCluster = Cells;
	EnemiesCountToSpawn = EnemiesCount;

	if (DependsOnEnemies)
	{
		FIMGArenaGenerationHelper::GetArenaTilesMaxNum(TilesMaxNum,
		                                               CellsToTraverse.Num(),
		                                               EnemiesCountToSpawn,
		                                               EnemyMapArena->Data,
		                                               EnemyMapArena->MovementTypeToArenaTypeImpact);
		for (auto TileMaxNum : TilesMaxNum)
		{
			if (TileMaxNum.Value > 0) { continue; }
			for (FIMGCellData* Cell : CellsToTraverse) { Cell->Entropy.Remove(TileMaxNum.Key); }
		}
		for (EIMGArenaTileType TileType : TEnumRange<EIMGArenaTileType>()) { TilesSpawned.Add(TileType, 0); }
	}
	
	// Setup Start and Fortress
	const FVector StartLocation = PathSpline->GetLocationAtTime(0.f, ESplineCoordinateSpace::World);
	FIMGCellData* StartTileCell = GetCellClosestToPoint(StartLocation);
	CollapseToType(StartTileCell, EIMGArenaTileType::EATT_Start);

	FIMGClusterData* StartClusterData = new FIMGClusterData{};
	StartClusterData->Type = EIMGArenaTileType::EATT_Start;
	StartClusterData->Cells.Add(StartTileCell);
	StartTileCell->IsInCluster = true;
	ClustersData.Add(StartClusterData);
	CellsToJoinIntoCluster.Remove(StartTileCell);

	// Each Tile around Start is Fortress
	FIMGClusterData* FortressClusterData = new FIMGClusterData{};
	FortressClusterData->Type = EIMGArenaTileType::EATT_Fortress;
	for (FIMGCellData* NeighborCell : StartTileCell->NeighborsPtrs)
	{
		CollapseToType(NeighborCell, EIMGArenaTileType::EATT_Fortress);
		FortressClusterData->Cells.Add(NeighborCell);
		NeighborCell->IsInCluster = true;
		CellsToJoinIntoCluster.Remove(NeighborCell);
	}
	ClustersData.Add(FortressClusterData);
	for (FIMGCellData* NeighborCells : StartTileCell->NeighborsPtrs) { Propagate(NeighborCells); }

	// Setup End 
	const FVector EndLocation = PathSpline->GetLocationAtTime(PathSpline->Duration, ESplineCoordinateSpace::World);
	FIMGCellData* EndTileCell = GetCellClosestToPoint(EndLocation);
	if (EndTileCell != StartTileCell)
	{
		CollapseToType(EndTileCell, EIMGArenaTileType::EATT_End);
		Type = EIMGArenaType::EAT_Corridor;

		FIMGClusterData* EndClusterData = new FIMGClusterData{};
		EndClusterData->Type = EIMGArenaTileType::EATT_End;
		EndClusterData->Cells.Add(EndTileCell);
		EndTileCell->IsInCluster = true;
		ClustersData.Add(EndClusterData);
		CellsToJoinIntoCluster.Remove(EndTileCell);
	}

	// Traversal of tiles
	while (CellsToTraverse.Num() > 0)
	{
		FIMGCellData* CellToCollapse = GetCellWithLeastEntropy();
		CollapseMesoPatterns(CellToCollapse);
		Propagate(CellToCollapse);
	}

	// Join into clusters
	while (CellsToJoinIntoCluster.Num() > 0) { JoinIntoClusters(CellsToJoinIntoCluster[0]); }
	ApplyHeightToClusters();
}

void FIMGArenaFloorGenerator::GetPerTileProcMeshBuildData(TArray<FIMGExtrudeData>& ExtrudeData)
{
	for (int32 CellIndex = 0; CellIndex < Cells.Num(); ++CellIndex)
	{
		// Sorting vertices around center clockwise
		FVector SiteCenter = Cells[CellIndex]->Site;
		UIMGCustomMath::SortClockwise(SiteCenter, Cells[CellIndex]->Vertices);

		FIMGExtrudeData Data;
		Data.TileType = Cells[CellIndex]->Type;
		for (int32 i = Cells[CellIndex]->Vertices.Num() - 1; i >= 0; --i)
		{
			Data.Vertices.Add(FVector2D(Cells[CellIndex]->Vertices[i].X, Cells[CellIndex]->Vertices[i].Y));
		}

		Data.Height = SiteCenter.Z;
		ExtrudeData.Add(Data);
	}
}

FIMGCellData* FIMGArenaFloorGenerator::GetCellClosestToPoint(const FVector& Point)
{
	if (Cells.Num() <= 0)
	{
		UE_LOG(LogIMGTilesdVoronoiArenaFloor, Error, TEXT("Num of Cells is equal or below 0"));
		return nullptr;
	}

	FIMGCellData* FoundCell = nullptr;
	float FoundDistance = TNumericLimits<float>::Max();
	for (FIMGCellData* Cell : Cells)
	{
		float Distance = FVector::Distance(Cell->Site, Point);
		if (Distance < FoundDistance)
		{
			FoundDistance = Distance;
			FoundCell = Cell;
		}
	}

	return FoundCell;
}

void FIMGArenaFloorGenerator::CollapseMesoPatterns(FIMGCellData* Cell)
{
	ApplyLocationModifiers(Cell);
	ApplySynergyModifiers(Cell);
	auto EntropyArray = Cell->Entropy.Array();
	EntropyArray.RemoveAll([&](TTuple<EIMGArenaTileType, float> A) { return A.Value <= 0.0000001f; });
	if (EntropyArray.Num() == 0)
	{
		UE_LOG(LogIMGTilesdVoronoiArenaFloor, Warning, TEXT("Met tile with 0 entropy"));
		CollapseToType(Cell, EIMGArenaTileType::EATT_Battle);
		return;
	}
	EIMGArenaTileType SelectedType =
		UIMGCustomMath::GetBiggestWeightClassFromTupleArray<EIMGArenaTileType>(EntropyArray);
	CollapseToType(Cell, SelectedType);
}

void FIMGArenaFloorGenerator::CollapseToType(FIMGCellData* Cell, EIMGArenaTileType TileType)
{
	Cell->Type = TileType;
	Cell->Entropy.Empty();
	CellsToTraverse.Remove(Cell);

	if (DependsOnEnemies)
	{
		*TilesSpawned.Find(TileType) += 1;
		CheckAndRemoveOverflowedTileTypesFromEntropy(TileType);
	}
}

void FIMGArenaFloorGenerator::CheckAndRemoveOverflowedTileTypesFromEntropy(EIMGArenaTileType TileType)
{
	const int32* TypeTilesMaxNum = TilesMaxNum.Find(TileType);
	const int32* TypeTilesSpawnedNum = TilesSpawned.Find(TileType);

	if (!TypeTilesMaxNum || !TypeTilesSpawnedNum) { return; }
	if (*TypeTilesMaxNum > *TypeTilesSpawnedNum) { return; }

	for (FIMGCellData* Cell : CellsToTraverse) { Cell->Entropy.Remove(TileType); }
}

void FIMGArenaFloorGenerator::Propagate(FIMGCellData* Cell)
{
	const auto Rules = MesoPatternCollapseRules->PatternRules.Find(Cell->Type);
	if (!Rules) { return; }

	const auto CellRules = Rules->NeighborsMap;
	switch (Cell->Type)
	{
		case EIMGArenaTileType::EATT_Empty:
		case EIMGArenaTileType::EATT_End:
		case EIMGArenaTileType::EATT_MAX:
			UE_LOG(LogIMGTilesdVoronoiArenaFloor, Warning, TEXT("Can't propagete for this type"));
			break;

		default:
			for (FIMGCellData* Neighbor : Cell->NeighborsPtrs)
			{
				if (Neighbor->Entropy.Num() == 0) { continue; }
				TMap<EIMGArenaTileType, float> NewNeighborEntropy;

				for (const auto Rule : CellRules)
				{
					if (float* Value = Neighbor->Entropy.Find(Rule.Key))
					{
						float NewValue = *Value + Rule.Value;
						NewNeighborEntropy.Add(Rule.Key, NewValue);
					}
				}
				Neighbor->Entropy = NewNeighborEntropy;
			}
			break;
	}
}

FIMGCellData* FIMGArenaFloorGenerator::GetCellWithLeastEntropy()
{
	FIMGCellData* CellWithLeastEntropy = nullptr;
	int32 LeastEntropy = static_cast<int32>(EIMGArenaTileType::EATT_MAX) + 1;
	for (FIMGCellData* Cell : CellsToTraverse)
	{
		if (Cell->Entropy.Num() < LeastEntropy)
		{
			CellWithLeastEntropy = Cell;
			LeastEntropy = Cell->Entropy.Num();
		}
	}

	return CellWithLeastEntropy;
}

void FIMGArenaFloorGenerator::ApplyLocationModifiers(FIMGCellData* Cell)
{
	for (const auto Entropy : Cell->Entropy)
	{
		float& Weight = *Cell->Entropy.Find(Entropy.Key);
		const FIMGTileWeightsMap* Rules = MesoPatternCollapseRules->PatternRules.Find(Entropy.Key);

		// =========================!!!HOTFIX THAT MUST BE REMOVED!!!===================================
		// =============================================================================================
		if (Entropy.Key == EIMGArenaTileType::EATT_AlternativePath && Type == EIMGArenaType::EAT_Arena)
		{
			Weight = 0.f;
			continue;
		}
		// =============================================================================================
		if (!Weight || !Rules || !Rules->GlobalLocationRules || Weight == 0) { continue; }

		auto* PathSpline = ParentArena->InnerBoundsSpline;
		auto* OuterBoundsSpline = ParentArena->OuterBoundsSpline;
		Weight *= Rules->GlobalLocationRules->GetMultiplierForPoint(Cell->Site, OuterBoundsSpline, PathSpline, Type);
	}
}

void FIMGArenaFloorGenerator::ApplySynergyModifiers(FIMGCellData* Cell)
{
	for (FIMGCellData* Neighbor : Cell->NeighborsPtrs)
	{
		if (Neighbor->Type == EIMGArenaTileType::EATT_Empty) { continue; }

		for (const auto Entropy : Cell->Entropy)
		{
			auto& Weight = *Cell->Entropy.Find(Entropy.Key);
			FIMGTileWeightsMap* Rules = MesoPatternCollapseRules->PatternRules.Find(Entropy.Key);
			if (!Weight || !Rules || Weight == 0) { continue; }

			UIMGTileSynergyRule** SynergyRule = Rules->TileSynergyRules.Find(Entropy.Key);
			if (!SynergyRule || !*SynergyRule) { continue; }

			USplineComponent* PathSpline = ParentArena->InnerBoundsSpline;
			USplineComponent* OuterBoundsSpline = ParentArena->OuterBoundsSpline;
			Weight *= (*SynergyRule)->GetMultiplierForNeighbor(Cell->Site,
			                                                   Neighbor->Site,
			                                                   OuterBoundsSpline,
			                                                   PathSpline);
		}
	}
}

void FIMGArenaFloorGenerator::JoinIntoClusters(FIMGCellData* StartCell)
{
	BufferCluster = new FIMGClusterData{};
	BufferCluster->Type = StartCell->Type;
	JoinAllTypedNeighboursIntoCluster(StartCell);
	ClustersData.Add(BufferCluster);
}

void FIMGArenaFloorGenerator::JoinAllTypedNeighboursIntoCluster(FIMGCellData* Cell)
{
	BufferCluster->Cells.Add(Cell);
	Cell->IsInCluster = true;
	CellsToJoinIntoCluster.Remove(Cell);

	for (FIMGCellData* Neighbor : Cell->NeighborsPtrs)
	{
		if (Neighbor->IsInCluster || Neighbor->Type != BufferCluster->Type) { continue; }
		JoinAllTypedNeighboursIntoCluster(Neighbor);
	}
}

void FIMGArenaFloorGenerator::ApplyHeightToClusters()
{
	for (const auto ClusterData : ClustersData)
	{
		float Height = 0.f;

		switch (ClusterData->Type)
		{
			case EIMGArenaTileType::EATT_Start:
			case EIMGArenaTileType::EATT_End:
			case EIMGArenaTileType::EATT_Fortress:
			case EIMGArenaTileType::EATT_Battle:
			case EIMGArenaTileType::EATT_ChokePoint:
			case EIMGArenaTileType::EATT_AlternativePath:
				Height = FMath::RandRange(1000.f, 1100.f);
				break;
			case EIMGArenaTileType::EATT_SniperPosition:
				Height = FMath::RandRange(1300.f, 1400.f);
				break;
			case EIMGArenaTileType::EATT_Gallery:
				Height = FMath::RandRange(1200.f, 1300.f);
				break;
			case EIMGArenaTileType::EATT_Blocked:
				Height = 3000.f;
				break;
		}

		for (auto Cell : ClusterData->Cells)
		{
			Cell->Site.Z += Height;
			for (auto& CellVertex : Cell->Vertices) { CellVertex.Z += Height; }
		}
	}
}

void FIMGArenaFloorGenerator::DebugTileTypes(const UWorld* World)
{
	FColor Color;

	for (const auto& Cell : Cells)
	{
		switch (Cell->Type)
		{
			case EIMGArenaTileType::EATT_Empty:
				Color = FColor::White;
				break;
			case EIMGArenaTileType::EATT_Start:
				Color = FColor::Red;
				break;
			case EIMGArenaTileType::EATT_End:
				Color = FColor::Magenta;
				break;
			case EIMGArenaTileType::EATT_SniperPosition:
				Color = FColor::Blue;
				break;
			case EIMGArenaTileType::EATT_Gallery:
				Color = FColor::Cyan;
				break;
			case EIMGArenaTileType::EATT_ChokePoint:
				Color = FColor::Orange;
				break;
			case EIMGArenaTileType::EATT_Fortress:
				Color = FColor::Yellow;
				break;
			case EIMGArenaTileType::EATT_Battle:
				Color = FColor::Green;
				break;
			case EIMGArenaTileType::EATT_AlternativePath:
				Color = FColor(118, 255, 122);
				break;
			case EIMGArenaTileType::EATT_Blocked:
				Color = FColor::Black;
				break;
			case EIMGArenaTileType::EATT_MAX:
				Color = FColor::Silver;
		}

		DrawDebugSphere(World, Cell->Site, 30.f, 6, Color, false, 1000000, 0, 60.f);
	}
}

void FIMGArenaFloorGenerator::DebugClusters(const UWorld* World)
{
	for (int32 i = 0; i < ClustersData.Num(); ++i)
	{
		if (ClustersData[i]->Cells.Num() == 0) { continue; }
		for (FIMGCellData* Cell : ClustersData[i]->Cells)
		{
			if (!Cell) { continue; }
			DrawDebugString(World,
			                Cell->Site + FVector(0, 0, 1000),
			                FString::FromInt(i),
			                nullptr,
			                FColor::White,
			                100000);
		}
	}
}

// void FIMGArenaFloorGenerator::JoinIntoClusters(FIMGCellData* StartCell)
// {
// 	BufferCluster = {};
// 	BufferCluster.Type = StartCell->Type;
// 	ClustersData.Add(&BufferCluster);
//
// 	BufferCluster.Cells.Add(StartCell);
// 	StartCell->IsInCluster = true;
// 	CellsToJoinIntoCluster.Remove(StartCell);
// 	
// 	BufferCells.Add(StartCell);
// 	while(BufferCells.Num()>0)
// 	{
// 		const FIMGCellData* CellData = BufferCells.Pop();
// 		for (FIMGCellData* Neighbor : CellData->NeighborsPtrs)
// 		{
// 			if (Neighbor->IsInCluster || Neighbor->Type != BufferCluster.Type) { continue; }
// 			BufferCluster.Cells.Add(Neighbor);
// 			Neighbor->IsInCluster = true;
// 			CellsToJoinIntoCluster.Remove(Neighbor);
// 			
// 			BufferCells.Add(Neighbor);
// 		}
// 	}
// }
//
// void FIMGArenaFloorGenerator::JoinAllTypedNeighboursIntoCluster(FIMGCellData* Cell)
// {
// 	BufferCluster.Cells.Add(Cell);
// 	Cell->IsInCluster = true;
// 	CellsToJoinIntoCluster.Remove(Cell);
//
// 	for (FIMGCellData* Neighbor : Cell->NeighborsPtrs)
// 	{
// 		if (Neighbor->IsInCluster && Neighbor->Type != BufferCluster.Type) { continue; }
// 		JoinAllTypedNeighboursIntoCluster(Neighbor);
// 	}
// }


// void FIMGArenaFloorGenerator::GetClusterBounds(UWorld* World)
// {
// 	for (FIMGClusterData* ClusterData : ClustersData)
// 	{
// 		TArray<FVector> BorderVertices;
// 		TMap<FVector, int32> Vertices;
//
// 		for (FIMGCellData* Cell : ClusterData->Cells)
// 		{
// 			TArray<FVector> UniqueSiteVertices;
// 			for (FVector& SiteVertex : Cell->Vertices)
// 			{
// 				const FVector* FoundVertex = UniqueSiteVertices.FindByPredicate([&](const FVector& A)
// 				{
// 					return A.Equals(SiteVertex);
// 				});
//
// 				if (FoundVertex) { continue; }
// 				UniqueSiteVertices.Add(SiteVertex);
// 			}
//
// 			for (FVector& Vertex : UniqueSiteVertices)
// 			{
// 				UIMGCustomMath::RoundVectorTo3Digit(Vertex);
// 				int32* NumOfSameVertices = Vertices.Find(Vertex);
// 				if (NumOfSameVertices) { *NumOfSameVertices += 1; }
// 				else { Vertices.Add(Vertex, 1); }
// 			}
// 		}
//
// 		for (const auto Vertex : Vertices)
// 		{
// 			if (Vertex.Value > 2) { continue; }
// 			BorderVertices.Add(Vertex.Key);
// 		}
//
// 		FVector SiteCenter = ClusterData->Cells[0]->Site;
// 		UIMGCustomMath::SortClockwise(SiteCenter, BorderVertices);
//
// 		for (int32 i = 1; i < BorderVertices.Num(); ++i)
// 		{
// 			DrawDebugLine(World, BorderVertices[i - 1], BorderVertices[i], FColor::Magenta, false, 100000, 100, 4);
// 		}
// 	}
// }

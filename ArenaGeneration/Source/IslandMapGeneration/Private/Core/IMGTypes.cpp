#include "Core/IMGTypes.h"

#include "GeomTools.h"
#include "Kismet/KismetMathLibrary.h"

FIMGEnemyGenerationData::FIMGEnemyGenerationData()
{
	for (EIMGArenaTileType Type : TEnumRange<EIMGArenaTileType>()) { MesoPatternEfficiencyImpact.Add(Type, 0.f); }
}

FIMGCellData::FIMGCellData()
{
	for (EIMGArenaTileType TileType : TEnumRange<EIMGArenaTileType>()) { Entropy.Add(TileType, 0.f); }
}

FIMGCellData::FIMGCellData(const FVoronoiCellInfo& Copy): FVoronoiCellInfo(Copy)
{
	for (EIMGArenaTileType TileType : TEnumRange<EIMGArenaTileType>()) { Entropy.Add(TileType, 0.f); }
}

void FIMGClusterData::GetBoxAroundCluster(
	UObject* WorldContextObject,
	FVector& OutRectCenter,
	FRotator& OutRectRotation,
	float& OutSideLengthX,
	float& OutSideLengthY,
	bool bDebugDraw)
{
	TArray<FVector> ClusterVertices{};
	FVector Normal{0, 0, 1};
	for (FIMGCellData* Cell : Cells) { for (FVector& Vertex : Cell->Vertices) { ClusterVertices.Add(Vertex); } }
	UKismetMathLibrary::MinAreaRectangle(WorldContextObject,
	                                     ClusterVertices,
	                                     Normal,
	                                     OutRectCenter,
	                                     OutRectRotation,
	                                     OutSideLengthX,
	                                     OutSideLengthY,
	                                     bDebugDraw);
}

bool FIMGClusterData::IsPointInsideCluster(FVector Point)
{
	for (FIMGCellData* Cell : Cells)
	{
		TArray<FVector2D> Vertices2D{Cell->Vertices};
		if (FGeomTools2D::IsPointInPolygon(FVector2d(Point), Vertices2D)) { return true; }
	}

	return false;
}

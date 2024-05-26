// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/WFCActor.h"

#include "Components/BoxComponent.h"
#include "Core/WFCTile.h"
#include "Core/WFCTileData.h"
#include "Core/DataAssets/WFCTileSet.h"
#include "Core/Debug/WFCEntropyDebug.h"
#include "Math/SSCustomMath.h"

DEFINE_LOG_CATEGORY_STATIC(LogWFCActor, All, All)

AWFCActor::AWFCActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GridBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Grid bounds"));
	SetRootComponent(GridBounds);
}

void AWFCActor::BeginPlay() { Super::BeginPlay(); }

void AWFCActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (FWFCGridCell& Cell : Grid.InnerArray) { if (Cell.Tile) { Cell.Tile->Destroy(); } }
	Grid.Empty();

	Super::EndPlay(EndPlayReason);
}

void AWFCActor::SetupGrid(const UWFCTileSet* Set, const FVector& Bounds, const FVector& TilesScale,
                          TFunction<bool(FVector Point)> CheckTileIsValid)
{
	TileSet = Set;
	GridBounds->SetBoxExtent(Bounds);
	TileScale = TilesScale;

	TileClass = Set->TileClass;
	RawTileSize = Set->TileSize;
	Tiles = Set->Tiles;
	EmptyTileSide = Set->EmptyTileSide;

	TileSize = RawTileSize * TilesScale;

	GridSize.X = FMath::RoundToInt32(GridBounds->GetScaledBoxExtent().X / TileSize.X) + 1;
	GridSize.Y = FMath::RoundToInt32(GridBounds->GetScaledBoxExtent().Y / TileSize.Y) + 1;

	Grid.Init(FWFCGridCell{}, GridSize.X, GridSize.Y);

	for (FWFCGridCell& Cell : Grid.InnerArray)
	{
		for (const UWFCTileData* Tile : Tiles)
		{
			FWFCEntropy CellEntropy;
			CellEntropy.TileData = Tile;
			Cell.Entropy.Add(CellEntropy);
		}
	}

	for (int32 X = 0; X < GridSize.X; ++X)
	{
		for (int32 Y = 0; Y < GridSize.Y; ++Y)
		{
			if (X * Y >= Grid.InnerArray.Num())
			{
				UE_LOG(LogStats, Error, TEXT("PIZDEC"));
				return;
			}

			Grid.Get(X, Y).Coordinates = FIntVector2(X, Y);

			FVector TileWorldLocation = GetCellWorldLocation(X, Y);
			// DrawDebugSphere(
			// 	GetWorld(),
			// 	TileWorldLocation,
			// 	30.f,
			// 	4,
			// 	FColor::Black,
			// 	false,
			// 	99999,
			// 	0,
			// 	4
			// );
			if (CheckTileIsValid && CheckTileIsValid(TileWorldLocation)) { CellsToCollapse.Add(&Grid.Get(X, Y)); }
			else if (CheckTileIsValid) { BlockedCells.Add(&Grid.Get(X, Y)); }

			// else { CollapseCellToBlockedAt(Y, X); }

			// AWFCEntropyDebug* Debug = GetWorld()->SpawnActor<AWFCEntropyDebug>(AWFCEntropyDebug::StaticClass());
			// Grid.Get(X, Y).EntropyDebug = Debug;
			// Debug->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			// Debug->SetActorRelativeLocation(GetCellRelativeLocation(X, Y));
		}
	}

	for (FWFCGridCell* Cell : BlockedCells) { CollapseCellToBlockedAt(Cell->Coordinates.X, Cell->Coordinates.Y); }
}

void AWFCActor::Generate()
{
	while (CellsToCollapse.Num() > 0)
	{
		FWFCGridCell* CellToCollapse = GetCellWithLeastEntropy();
		CollapseCellAt(CellToCollapse->Coordinates.X, CellToCollapse->Coordinates.Y);
		if (CellToCollapse->Status != EWFCCellStatus::ECS_FailedToCollapse)
		{
			Propagate(*CellToCollapse, CellToCollapse->Coordinates.X, CellToCollapse->Coordinates.Y);
		}
		else { PropagateBlocked(*CellToCollapse, CellToCollapse->Coordinates.X, CellToCollapse->Coordinates.Y); }
	}
}

void AWFCActor::PerformStep()
{
	if (CellsToCollapse.Num() == 0) { return; }
	FlushDebugStrings(GetWorld());

	FWFCGridCell* CellToCollapse = GetCellWithLeastEntropy();
	CollapseCellAt(CellToCollapse->Coordinates.X, CellToCollapse->Coordinates.Y);
	if (CellToCollapse->Status != EWFCCellStatus::ECS_FailedToCollapse)
	{
		Propagate(*CellToCollapse, CellToCollapse->Coordinates.X, CellToCollapse->Coordinates.Y);
	}
	else { PropagateBlocked(*CellToCollapse, CellToCollapse->Coordinates.X, CellToCollapse->Coordinates.Y); }
	// DebugEntropy();
	// MarkNeighbors(CellToCollapse->Coordinates.X, CellToCollapse->Coordinates.Y);

	// FVector CellRelLoc = GetCellRelativeLocation(CellToCollapse->Coordinates.X, CellToCollapse->Coordinates.Y);
	// UE_LOG(LogWFCActor, Display, TEXT("%s"), *CellRelLoc.ToString());
}

void AWFCActor::DebugEntropy()
{
	for (FWFCGridCell& Cell : Grid.InnerArray)
	{
		FString EntropyString = "";
		for (const FWFCEntropy& Entropy : Cell.Entropy) { EntropyString.Append(Entropy.TileData->TileName + "\n"); }

		Cell.EntropyDebug->SetDebugText(EntropyString);

		// DrawDebugString(
		// 	GetWorld(),
		// 	GetCellWorldLocation(Cell.Coordinates.X, Cell.Coordinates.Y),
		// 	EntropyString,
		// 	GetWorld()->GetFirstPlayerController()->GetPawn(),
		// 	FColor::White,
		// 	9999,
		// 	false
		// )
	}
}

FVector AWFCActor::GetCellRelativeLocation(int32 X, int32 Y)
{
	FVector RelativeLocation(
		(X * TileSize.X) - GridBounds->GetUnscaledBoxExtent().X / 2,
		(Y * TileSize.Y) - GridBounds->GetUnscaledBoxExtent().Y / 2,
		0);

	// UE_LOG(LogWFCActor, Display, TEXT("Relative location for X: %d, Y: %d is %s"), X, Y, *RelativeLocation.ToString());
	return RelativeLocation;
}

FVector AWFCActor::GetCellWorldLocation(int32 X, int32 Y)
{
	const FTransform CellLocalTransform{-GetCellRelativeLocation(X, Y)};
	const FTransform CellWorldTransform = CellLocalTransform.GetRelativeTransformReverse(GetTransform());
	return CellWorldTransform.GetLocation();
}

FWFCGridCell* AWFCActor::GetCellWithLeastEntropy()
{
	FWFCGridCell* CellWithLeastEntropy = nullptr;
	int32 LeastEntropy = TileSet->Tiles.Num() + 1;
	for (FWFCGridCell* Cell : CellsToCollapse)
	{
		if (Cell->Entropy.Num() < LeastEntropy)
		{
			CellWithLeastEntropy = Cell;
			LeastEntropy = Cell->Entropy.Num();
		}
	}

	return CellWithLeastEntropy;
}

void AWFCActor::CollapseCellAt(int32 X, int32 Y)
{
	FWFCGridCell& Cell = Grid.Get(X, Y);
	if (Cell.Status == EWFCCellStatus::ECS_Blocked) { return; }
	if (Cell.Entropy.Num() == 0)
	{
		Cell.Status = EWFCCellStatus::ECS_FailedToCollapse;
		// DrawDebugSphere(
		// 	GetWorld(),
		// 	GetCellWorldLocation(X, Y),
		// 	100.f,
		// 	4,
		// 	FColor::Red,
		// 	false,
		// 	9999,
		// 	100,
		// 	2
		// );
		CellsToCollapse.Remove(&Cell);
		return;
	}
	const UWFCTileData* TileData = GetRandomTileFromEntropy(Cell.Entropy);

	AWFCTile* SpawnedTile = SpawnTileAt(X, Y, TileData);
	if (!SpawnedTile)
	{
		Cell.Status = EWFCCellStatus::ECS_FailedToCollapse;
		// DrawDebugSphere(
		// 	GetWorld(),
		// 	GetCellWorldLocation(X, Y),
		// 	100.f,
		// 	4,
		// 	FColor::Red,
		// 	false,
		// 	9999,
		// 	100,
		// 	2
		// );
		CellsToCollapse.Remove(&Cell);
		return;
	}

	Cell.Status = EWFCCellStatus::ECS_Collapsed;
	Cell.Tile = SpawnedTile;
	// DrawDebugSphere(
	// 	GetWorld(),
	// 	GetCellWorldLocation(X, Y),
	// 	100.f,
	// 	4,
	// 	FColor::Green,
	// 	false,
	// 	9999,
	// 	100,
	// 	2
	// );

	CellsToCollapse.Remove(&Cell);
}

void AWFCActor::CollapseCellToBlockedAt(int32 X, int32 Y)
{
	FWFCGridCell& Cell = Grid.Get(X, Y);
	Cell.Status = EWFCCellStatus::ECS_Blocked;
	Cell.Entropy.Empty();
	PropagateBlocked(Cell, X, Y);
}

void AWFCActor::Propagate(FWFCGridCell& Cell, int32 CellX, int32 CellY)
{
	for (EWFCOrientation Orientation : TEnumRange<EWFCOrientation>())
	{
		if (!DoesHaveNeighborFor(Orientation, CellX, CellY)) { continue; }
		FWFCGridCell& Neighbor = GetNeighborAt(Orientation, CellX, CellY);
		if (Neighbor.Status == EWFCCellStatus::ECS_Collapsed || Neighbor.Status == EWFCCellStatus::ECS_Blocked)
		{
			continue;
		}

		const bool* BlocksSide = Cell.Tile->TileData->IsFullySymmetrical
			                         ? &Cell.Tile->TileData->BloksSide
			                         : Cell.Tile->TileData->BlocksSides.Find(Orientation);
		if (*BlocksSide)
		{
			Neighbor.Entropy.RemoveAll([&](FWFCEntropy& Entropy)
			{
				return !Entropy.TileData->TileName.Equals(TileSet->EmptyTileName);
			});

			continue;
		}

		const FGameplayTag* CellSide = Cell.Tile->TileData->IsFullySymmetrical
			                               ? &Cell.Tile->TileData->Side
			                               : Cell.Tile->TileData->Sides.Find(Orientation);

		Neighbor.Entropy.RemoveAll([&](FWFCEntropy& Entropy)
		{
			const FGameplayTag* NeighborSide = Entropy.TileData->IsFullySymmetrical
				                                   ? &Entropy.TileData->Side
				                                   : Entropy.TileData->Sides.Find(GetOppositeSide(Orientation));
			if (!NeighborSide) { return true; }
			return *NeighborSide != *CellSide;
		});
	}
}

void AWFCActor::PropagateBlocked(FWFCGridCell& Cell, int32 CellX, int32 CellY)
{
	for (EWFCOrientation Orientation : TEnumRange<EWFCOrientation>())
	{
		if (!DoesHaveNeighborFor(Orientation, CellX, CellY)) { continue; }
		FWFCGridCell& Neighbor = GetNeighborAt(Orientation, CellX, CellY);

		Neighbor.Entropy.RemoveAll([&](FWFCEntropy& Entropy)
		{
			const FGameplayTag* NeighborSide = Entropy.TileData->IsFullySymmetrical
				                                   ? &Entropy.TileData->Side
				                                   : Entropy.TileData->Sides.Find(GetOppositeSide(Orientation));
			if (!NeighborSide) { return true; }
			return *NeighborSide != EmptyTileSide;
		});

		// DrawDebugSphere(
		// 	GetWorld(),
		// 	GetCellWorldLocation(CellY, CellX),
		// 	100.f,
		// 	4,
		// 	FColor::Black,
		// 	false,
		// 	9999,
		// 	100,
		// 	2
		// );
	}
}

bool AWFCActor::DoesHaveNeighborFor(EWFCOrientation Orientation, int32 CellX, int32 CellY)
{
	int32 X = CellX;
	int32 Y = CellY;
	switch (Orientation)
	{
		case EWFCOrientation::EO_Front:
			X += 1;
			break;
		case EWFCOrientation::EO_Right:
			Y += 1;
			break;
		case EWFCOrientation::EO_Back:
			X -= 1;
			break;
		case EWFCOrientation::EO_Left:
			Y -= 1;
			break;
	}

	if (X < 0 || Y < 0 || X >= Grid.GetD1() || Y >= Grid.GetD2()) { return false; }
	return true;
}

FWFCGridCell& AWFCActor::GetNeighborAt(EWFCOrientation Orientation, int32 CellX, int32 CellY)
{
	int32 X = CellX;
	int32 Y = CellY;
	switch (Orientation)
	{
		case EWFCOrientation::EO_Front:
			X += 1;
			break;
		case EWFCOrientation::EO_Right:
			Y += 1;
			break;
		case EWFCOrientation::EO_Back:
			X -= 1;
			break;
		case EWFCOrientation::EO_Left:
			Y -= 1;
			break;
	}

	// if (X < 0 || Y < 0 || X >= Grid.GetD2() || Y >= Grid.GetD1()) { return nullptr; }
	return Grid.Get(X, Y);
}

const UWFCTileData* AWFCActor::GetRandomTileFromEntropy(TArray<FWFCEntropy>& Entropy)
{
	TArray<float> Weights;
	for (int32 index = 0; index < Entropy.Num(); ++index) { Weights.Add(Entropy[index].TileData->Weight); }
	int32 RandomIndex = USSCustomMath::GetRandomWeightedIndex(Weights);

	return Entropy[RandomIndex].TileData;
}

AWFCTile* AWFCActor::SpawnTileAt(int32 X, int32 Y, const UWFCTileData* TileData)
{
	UWorld* World = GetWorld();
	if (!World) { return nullptr; }

	AWFCTile* Tile = World->SpawnActor<AWFCTile>(TileClass);
	Tile->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	FVector RelativeLocation = GetCellRelativeLocation(X, Y);
	// RelativeLocation.Z += TileSize.Z / 2;
	Tile->SetActorRelativeLocation(RelativeLocation);
	Tile->SetActorScale3D(TileSet->TileScale);

	FRotator TileRotation = FRotator(0.f, 0.f, 0.f);
	switch (TileData->Orientation)
	{
		case EWFCOrientation::EO_Front:
			TileRotation.Yaw = 0.f;
			break;
		case EWFCOrientation::EO_Right:
			TileRotation.Yaw = 90.f;
			break;
		case EWFCOrientation::EO_Back:
			TileRotation.Yaw = 180.f;
			break;
		case EWFCOrientation::EO_Left:
			TileRotation.Yaw = 270.f;
			break;
	}
	Tile->SetActorRelativeRotation(TileRotation);

	Tile->TileData = TileData;
	if (TileData->Meshes.Num() > 0)
	{
		int32 RandStaticMeshIndex = FMath::RandRange(0, TileData->Meshes.Num() - 1);
		UStaticMesh* Mesh = TileData->Meshes[RandStaticMeshIndex];
		Tile->Mesh->SetStaticMesh(Mesh);
	}

	return Tile;
}


void AWFCActor::MarkNeighbors(int32 X, int32 Y)
{
	DrawDebugSphere(GetWorld(), GetCellWorldLocation(X + 1, Y), 30.f, 4, FColor::Orange, false, 9999, 101); // Forward
	DrawDebugSphere(GetWorld(), GetCellWorldLocation(X, Y + 1), 30.f, 4, FColor::Blue, false, 9999, 101);   // Right
	DrawDebugSphere(GetWorld(), GetCellWorldLocation(X - 1, Y), 30.f, 4, FColor::Green, false, 9999, 101);  // Back
	DrawDebugSphere(GetWorld(), GetCellWorldLocation(X, Y - 1), 30.f, 4, FColor::Red, false, 9999, 101);    // Left
}

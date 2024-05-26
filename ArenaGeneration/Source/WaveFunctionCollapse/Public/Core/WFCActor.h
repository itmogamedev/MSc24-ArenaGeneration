// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WFCTypes.h"
#include "Core/TArray2D.h"
#include "GameFramework/Actor.h"
#include "WFCActor.generated.h"

class UWFCTileSet;
class UBoxComponent;

UCLASS()
class WAVEFUNCTIONCOLLAPSE_API AWFCActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WFC")
	TObjectPtr<UBoxComponent> GridBounds;

	UPROPERTY(BlueprintReadOnly, Category="WFC")
	const UWFCTileSet* TileSet;

	TArray2D<FWFCGridCell> Grid;

	AWFCActor();

	void SetupGrid(const UWFCTileSet* Set, const FVector& Bounds, const FVector& TilesScale = FVector(1, 1, 1),
	               TFunction<bool(FVector Point)> CheckTileIsValid = nullptr);

	UFUNCTION(BlueprintCallable, CallInEditor)
	void Generate();

	UFUNCTION(BlueprintCallable, CallInEditor)
	void PerformStep();

	void DebugEntropy();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	FGameplayTag EmptyTileSide;

	FIntVector2 GridSize = FIntVector2(0, 0);
	FVector TileScale{1.f, 1.f, 1.f};

	FVector TileSize = FVector::One();
	FVector RawTileSize;

	TSubclassOf<AWFCTile> TileClass;
	TArray<const UWFCTileData*> Tiles;

	TArray<FWFCGridCell*> CellsToCollapse;
	TArray<FWFCGridCell*> BlockedCells;

	FVector GetCellRelativeLocation(int32 X, int32 Y);
	FVector GetCellWorldLocation(int32 X, int32 Y);

	FWFCGridCell* GetCellWithLeastEntropy();

	void CollapseCellAt(int32 X, int32 Y);
	void CollapseCellToBlockedAt(int32 X, int32 Y);

	void Propagate(FWFCGridCell& Cell, int32 CellX, int32 CellY);
	void PropagateBlocked(FWFCGridCell& Cell, int32 CellX, int32 CellY);

	bool DoesHaveNeighborFor(EWFCOrientation Orientation, int32 CellX, int32 CellY);
	FWFCGridCell& GetNeighborAt(EWFCOrientation Orientation, int32 CellX, int32 CellY);

	const UWFCTileData* GetRandomTileFromEntropy(TArray<FWFCEntropy>& Entropy);

	AWFCTile* SpawnTileAt(int32 X, int32 Y, const UWFCTileData* TileData);
	void MarkNeighbors(int32 X, int32 Y);
};

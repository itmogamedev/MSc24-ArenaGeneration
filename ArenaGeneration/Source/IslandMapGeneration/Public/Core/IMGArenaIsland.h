// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IMGIsland.h"
#include "ProceduralMeshComponent.h"
#include "Arena/IMGArenaFloorGenerator.h"
#include "IMGArenaIsland.generated.h"

class UEnemy;
class UIMGTileSets;
class UDynamicMeshComponent;
class AArena;
class UIMGSplineCollider;
class UBoxComponent;
class UProceduralMesh;
class FIMGArenaFloorGenerator;

UCLASS()
class ISLANDMAPGENERATION_API AIMGArenaIsland : public AIMGIsland
{
	GENERATED_BODY()

public:
	AIMGArenaIsland();
	friend FIMGArenaFloorGenerator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Arena generation")
	bool DependsOnEnemies = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Arena generation")
	UIMGMesoPatternBuildRules* CollapseRules;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Arena generation")
	UIMGTileSets* TileSets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Arena generation")
	TObjectPtr<UBoxComponent> ArenaPointsScatterBounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Arena generation")
	TObjectPtr<UIMGSplineCollider> ArenaFloorBounds;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ArenaGeneration")
	TSubclassOf<AWFCActor> WFCActorClass;

	UFUNCTION(BlueprintCallable, Category="Arena generation")
	void GenerateArenaGeometry(TMap<TSubclassOf<UEnemy>, int32> EnemiesToSpawn,
	                           UIMGEnemyMapArenaGenerationDB* EnemyMapArena);

	TArray<FVector> SpawnPoints{};
	FVector PlayerSpawnPoint = FVector::ZeroVector;
	FVector EndLevel = FVector::ZeroVector;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UProceduralMeshComponent> FloorProceduralMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UDynamicMeshComponent> FloorDynamicMesh;

	UPROPERTY(BlueprintReadOnly)
	TArray<FIMGExtrudeData> ExtrudeData;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Arena generation")
	int32 ScatterPointsNum = 100;

	FIMGArenaFloorGenerator ArenaFloorGenerator;

	UFUNCTION(BlueprintCallable, Category="Arena generation")
	void GenerateFloor(TMap<TSubclassOf<UEnemy>, int32> EnemiesToSpawn,
	                   UIMGEnemyMapArenaGenerationDB* EnemyMapArena);
};

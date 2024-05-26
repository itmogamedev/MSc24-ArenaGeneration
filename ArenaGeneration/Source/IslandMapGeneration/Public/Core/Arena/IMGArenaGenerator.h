// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/IMGTypes.h"
#include "GameFramework/Actor.h"
#include "IMGArenaGenerator.generated.h"

class UIMGEnemyMapArenaGenerationDB;
class AIMGArenaIsland;
class AArena;
class AExampleEnemy;

USTRUCT(BlueprintType)
struct ISLANDMAPGENERATION_API FIMGArenasPool
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<AIMGArenaIsland>> Array;
};

UCLASS()
class ISLANDMAPGENERATION_API AIMGArenaGenerator : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DependsOnEnemies = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RandomDependsOnEnemies = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<AExampleEnemy>, int32> EnemiesToSpawn;
	
	AIMGArenaGenerator();

	UFUNCTION(BlueprintCallable, CallInEditor)
	void GenerateArena();

	UFUNCTION(BlueprintCallable, CallInEditor)
	void RemoveCurrentArena();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EIMGArenaType, FIMGArenasPool> ArenasPool;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UIMGEnemyMapArenaGenerationDB* EnemyMapArena;

private:
	UPROPERTY()
	AIMGArenaIsland* CurrentArenaIsland;
};

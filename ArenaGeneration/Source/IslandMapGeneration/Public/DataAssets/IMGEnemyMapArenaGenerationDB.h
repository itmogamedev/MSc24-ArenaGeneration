// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/IMGTypes.h"
#include "Engine/DataAsset.h"
#include "IMGEnemyMapArenaGenerationDB.generated.h"

class AGHBaseEnemy;
/**
 * 
 */
UCLASS()
class ISLANDMAPGENERATION_API UIMGEnemyMapArenaGenerationDB : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EIMGEnemyMovementType, FIMGTileTypeToMultiplier> MovementTypeToArenaTypeImpact;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<TSubclassOf<AExampleEnemy>, FIMGEnemyGenerationData> Data;
};

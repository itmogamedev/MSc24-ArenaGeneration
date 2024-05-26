// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Core/WFCTypes.h"
#include "Engine/DataAsset.h"
#include "WFCTileSet.generated.h"

class UWFCSideEnum;
class UWFCTileData;
class AWFCTile;
/**
 * 
 */
UCLASS()
class WAVEFUNCTIONCOLLAPSE_API UWFCTileSet : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector TileSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector TileScale;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AWFCTile> TileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag EmptyTileSide;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString EmptyTileName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag BlockedTileSide;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<const UWFCTileData*> Tiles;
};

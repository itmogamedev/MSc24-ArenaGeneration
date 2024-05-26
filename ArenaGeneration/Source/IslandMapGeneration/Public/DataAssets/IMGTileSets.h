// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/IMGTypes.h"
#include "Engine/DataAsset.h"
#include "IMGTileSets.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAPGENERATION_API UIMGTileSets : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<EIMGArenaTileType, UWFCTileSet*> TileSets;

	UIMGTileSets();
};

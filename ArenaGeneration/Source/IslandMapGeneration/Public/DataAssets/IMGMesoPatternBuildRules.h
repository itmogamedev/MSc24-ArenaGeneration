// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/IMGTypes.h"
#include "Engine/DataAsset.h"
#include "IMGMesoPatternBuildRules.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDMAPGENERATION_API UIMGMesoPatternBuildRules : public UDataAsset
{
	GENERATED_BODY()

	UIMGMesoPatternBuildRules();

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EIMGArenaTileType, FIMGTileWeightsMap> PatternRules;
};

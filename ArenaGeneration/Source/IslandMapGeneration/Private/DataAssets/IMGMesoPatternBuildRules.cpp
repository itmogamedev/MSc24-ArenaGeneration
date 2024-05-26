// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/IMGMesoPatternBuildRules.h"

UIMGMesoPatternBuildRules::UIMGMesoPatternBuildRules()
{
	for (const auto Type : TEnumRange<EIMGArenaTileType>()) { PatternRules.Add(Type, FIMGTileWeightsMap()); }
}

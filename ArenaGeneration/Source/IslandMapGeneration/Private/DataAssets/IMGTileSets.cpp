// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/IMGTileSets.h"

UIMGTileSets::UIMGTileSets()
{
	for(EIMGArenaTileType TileType : TEnumRange<EIMGArenaTileType>())
	{
		TileSets.Add(TileType, nullptr);
	}
}

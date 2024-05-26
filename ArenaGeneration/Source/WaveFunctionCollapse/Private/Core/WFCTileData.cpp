// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/WFCTileData.h"

UWFCTileData::UWFCTileData()
{
	for (EWFCOrientation SideOrientation : TEnumRange<EWFCOrientation>())
	{
		Sides.Add(SideOrientation, FGameplayTag());
		BlocksSides.Add(SideOrientation, false);
	}
}

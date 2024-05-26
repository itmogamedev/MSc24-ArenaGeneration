// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IMGIsland.h"
#include "IMGArenaHubIsland.generated.h"

UCLASS()
class ISLANDMAPGENERATION_API AIMGArenaHubIsland : public AIMGIsland
{
	GENERATED_BODY()

public:
	AIMGArenaHubIsland();

protected:
	virtual void BeginPlay() override;

};

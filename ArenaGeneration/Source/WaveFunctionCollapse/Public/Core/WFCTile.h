// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WFCTypes.h"
#include "GameFramework/Actor.h"
#include "WFCTile.generated.h"

class UWFCTileData;

UCLASS()
class WAVEFUNCTIONCOLLAPSE_API AWFCTile : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	const UWFCTileData* TileData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EWFCOrientation Orientation = EWFCOrientation::EO_Front;

	AWFCTile();

protected:
	virtual void BeginPlay() override;
};

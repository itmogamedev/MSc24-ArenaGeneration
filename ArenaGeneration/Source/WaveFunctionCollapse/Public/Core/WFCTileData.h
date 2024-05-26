// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WFCTypes.h"
#include "UObject/Object.h"
#include "WFCTileData.generated.h"


UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class WAVEFUNCTIONCOLLAPSE_API UWFCTileData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString TileName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<UStaticMesh*> Meshes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool IsFullySymmetrical = false;

	// X+ - Front
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EWFCOrientation Orientation = EWFCOrientation::EO_Front;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition = "IsFullySymmetrical", Get))
	FGameplayTag Side;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition = "IsFullySymmetrical", Get))
	bool BloksSide;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition = "!IsFullySymmetrical"))
	TMap<EWFCOrientation, FGameplayTag> Sides;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(EditCondition = "!IsFullySymmetrical"))
	TMap<EWFCOrientation, bool> BlocksSides;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Weight = 1.f;

	UWFCTileData();
};

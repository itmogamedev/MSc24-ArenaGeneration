#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WFCTypes.generated.h"

class AWFCEntropyDebug;
class UWFCTileData;
class AWFCTile;

// Front - Positive X direction
UENUM(BlueprintType)
enum class EWFCOrientation : uint8
{
	EO_Front UMETA(DisplayName = "Front"),
	EO_Right UMETA(DisplayName = "Right"),
	EO_Back UMETA(DisplayName = "Back"),
	EO_Left UMETA(DisplayName = "Left"),

	EO_MAX UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EWFCOrientation, EWFCOrientation::EO_MAX);

FORCEINLINE static EWFCOrientation GetOppositeSide(EWFCOrientation Side)
{
	switch (Side)
	{
		case EWFCOrientation::EO_Front:
			return EWFCOrientation::EO_Back;
		case EWFCOrientation::EO_Right:
			return EWFCOrientation::EO_Left;
		case EWFCOrientation::EO_Back:
			return EWFCOrientation::EO_Front;
		case EWFCOrientation::EO_Left:
			return EWFCOrientation::EO_Right;
		case EWFCOrientation::EO_MAX:
			return EWFCOrientation::EO_MAX;
	}
	return EWFCOrientation::EO_MAX;
};

UENUM(BlueprintType)
enum class EWFCCellStatus : uint8
{
	ECS_Default UMETA(DisplayName = "Default"),
	ECS_Collapsed UMETA(DisplayName = "Collapsed"),
	ECS_FailedToCollapse UMETA(DisplayName = "Failed to collapse"),
	ECS_Blocked UMETA(DisplayName = "Blocked"),

	ECS_MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct WAVEFUNCTIONCOLLAPSE_API FWFCEntropy
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	const UWFCTileData* TileData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSet<EWFCOrientation> Set = {
		EWFCOrientation::EO_Front,
		EWFCOrientation::EO_Right,
		EWFCOrientation::EO_Back,
		EWFCOrientation::EO_Left
	};
};

USTRUCT(BlueprintType)
struct WAVEFUNCTIONCOLLAPSE_API FWFCGridCell
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EWFCCellStatus Status = EWFCCellStatus::ECS_Default;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FWFCEntropy> Entropy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AWFCTile* Tile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AWFCEntropyDebug* EntropyDebug;

	FIntVector2 Coordinates{0, 0};
	TMap<EWFCOrientation, FGameplayTag> NeighborsSides;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Arena/TileSynergyRules/IMGTileSynergyRule.h"
#include "Core/WFCActor.h"
#include "Voronoi/Voronoi.h"
#include "IMGTypes.generated.h"

class UIMGGlobalLocationRule;
class UStaticMesh;
class AIMGIsland;
class AIMGDebugActor;
class AIMGIslandProp;

// Islands ENUMs
UENUM(BlueprintType)
enum class EIMGStage : uint8
{
	ES_Stage1 UMETA(DisplayName = "Stage 1"),
	ES_Stage2 UMETA(DisplayName = "Stage 2"),
	ES_Stage3 UMETA(DisplayName = "Stage 3"),
	ES_Stage4 UMETA(DisplayName = "Stage 4"),
	ES_Stage5 UMETA(DisplayName = "Stage 5"),
	ES_Stage6 UMETA(DisplayName = "Stage 6"),
	ES_Stage7 UMETA(DisplayName = "Stage 7"),
	ES_Stage8 UMETA(DisplayName = "Stage 8"),
	ES_Stage9 UMETA(DisplayName = "Stage 9"),
	ES_Stage10 UMETA(DisplayName = "Stage 10")
};

UENUM(BlueprintType)
enum class EIMGIslandType : uint8
{
	EIT_Start UMETA(DisplayName = "Start island"),
	EIT_Loot UMETA(DisplayName = "Loot island"),
	EIT_Arena UMETA(DisplayName = "Arena island"),
	EIT_Boss UMETA(DisplayName = "Boss island"),

	EIT_MAX UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EIMGIslandType, EIMGIslandType::EIT_Boss);

UENUM(BlueprintType)
enum class EIMGIslandSize : uint8
{
	EIS_Small UMETA(DisplayName = "Small island"),
	EIS_Mid UMETA(DisplayName = "Mid island"),
	EIS_Big UMETA(DisplayName = "Big island"),

	EIS_MAX UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EIMGIslandSize, EIMGIslandSize::EIS_MAX);

UENUM(BlueprintType)
enum class EIMGOverlapType : uint8
{
	EOT_NotOverlapping UMETA(DisplayName = "Not overlapping"),
	EOT_OnlyWithSameClass UMETA(DisplayName = "With it's class instances"),
	EOT_Overlapping UMETA(DisplayName = "Overlapping"),

	EOT_MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EIMGSpawnType : uint8
{
	EST_InsideIsland UMETA(DisplayName = "Inside island"),
	EST_OnEdge UMETA(DisplayName = "On edge"),

	EST_MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EIMGPropType : uint8
{
	EAT_Hill UMETA(DisplayName = "Hill"),
	EAT_Bridge UMETA(DisplayName = "Bridge"),
	EAT_Stalactites UMETA(DisplayName = "Stalactite"),
	EAT_Cliff UMETA(DisplayName = "Cliff"),
	EAT_Building UMETA(DisplayName = "Building"),
	EAT_Fooliage UMETA(DisplayName = "Fooliage"),
	EAT_Rocks UMETA(DisplayName = "Rock"),
	EAT_Seller UMETA(DisplayName = "Seller"),
	EAT_Defence UMETA(DisplayName = "Defence"),
	EAT_Utilities UMETA(DisplayName = "Utilities"),

	EAT_MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EIMGGridCellStatus : uint8
{
	EGCS_Inactive UMETA(DisplayName = "Inactive"),
	EGCS_Active UMETA(DisplayName = "Active"),
	EGCS_Filled UMETA(DisplayName = "Filled"),
	EGCS_FailedToFill UMETA(DisplayName = "Failed To Fill"),

	EGCS_MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct ISLANDMAPGENERATION_API FIMGGridCell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EIMGGridCellStatus Status = EIMGGridCellStatus::EGCS_Inactive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AIMGIsland* Island = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AIMGDebugActor* DebugActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DistanceFromH = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FIntVector Location = FIntVector(0);
};

FORCEINLINE bool operator==(const FIMGGridCell& A, const FIMGGridCell& B) { return A.Location == B.Location; }

USTRUCT(BlueprintType)
struct ISLANDMAPGENERATION_API FIMGIslandData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AIMGIsland> IslandClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f))
	float SpawnWeight;
};

#pragma region ArenaTypes
UENUM(BlueprintType)
enum class EIMGArenaType : uint8
{
	EAT_Arena UMETA(DisplayName = "Arena"),
	EAT_Corridor UMETA(DisplayName = "Corridor"),

	EAT_MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EIMGArenaTileType : uint8
{
	EATT_Empty UMETA(DisplayName = "Empty"),
	EATT_Start UMETA(DisplayName = "Start"),
	EATT_End UMETA(DisplayName = "End"),
	EATT_SniperPosition UMETA(DisplayName = "Sniper position"),
	EATT_Gallery UMETA(DisplayName = "Gallery"),
	EATT_ChokePoint UMETA(DisplayName = "ChokePoint"),
	EATT_Fortress UMETA(DisplayName = "Fortress"),
	EATT_Battle UMETA(DisplayName = "Battle"),
	EATT_AlternativePath UMETA(DisplayName = "AlternativePath"),
	EATT_Blocked UMETA(DisplayName = "Blocked"),

	EATT_MAX UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EIMGArenaTileType, EIMGArenaTileType::EATT_MAX);

UENUM(BlueprintType)
enum class EIMGEnemyMovementType : uint8
{
	EEMT_Default UMETA(DisplayName = "Default"),
	EEMT_Flanking UMETA(DisplayName = "Flanking"),
	EEMT_Passive UMETA(DisplayName = "Passive"),
	EEMT_Slow UMETA(DisplayName = "Slow"),
	EEMT_Cautious UMETA(DisplayName = "Cautious"),

	EEMT_MAX UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EIMGEnemyMovementType, EIMGEnemyMovementType::EEMT_MAX);

USTRUCT(BlueprintType)
struct ISLANDMAPGENERATION_API FIMGEnemyGenerationData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f))
	float Efficiency = 1.f;

	// 0 is for wide arena, 1 is for narrow corridor 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, ClampMax = 1.f))
	float PreferredArenaWideness = .5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EIMGEnemyMovementType MovementType = EIMGEnemyMovementType::EEMT_Default;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = -1.f, ClampMax = 1.f))
	TMap<EIMGArenaTileType, float> MesoPatternEfficiencyImpact;

	FIMGEnemyGenerationData();
};

USTRUCT(BlueprintType)
struct ISLANDMAPGENERATION_API FIMGTileTypeToMultiplier
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = -1.f, UIMax = 1.f, ClampMin = -1.f, ClampMax = 1.f))
	TMap<EIMGArenaTileType, float> MultiplierMap;
};


struct ISLANDMAPGENERATION_API FIMGCellData : FVoronoiCellInfo
{
	FVector Site;
	EIMGArenaTileType Type = EIMGArenaTileType::EATT_Empty;
	TMap<EIMGArenaTileType, float> Entropy;
	TArray<FIMGCellData*> NeighborsPtrs;

	bool IsInCluster = false;

	FIMGCellData();
	FIMGCellData(const FVoronoiCellInfo& Copy);
};

struct ISLANDMAPGENERATION_API FIMGCellsArray
{
	TArray<FIMGCellData*> Array;
};

USTRUCT(BlueprintType)
struct ISLANDMAPGENERATION_API FIMGTileWeightsMap
{
	GENERATED_BODY()

	/**
	 *	@note If value is in the Map then tile type can be set nearby. Value uses when next tile tries to mark it's neighbor
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = 0.f, UIMax = 1.f, ClampMin = 0.f, ClampMax = 1.f))
	TMap<EIMGArenaTileType, float> NeighborsMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	UIMGGlobalLocationRule* GlobalLocationRules;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TMap<EIMGArenaTileType, UIMGTileSynergyRule*> TileSynergyRules;
};


struct ISLANDMAPGENERATION_API FIMGClusterData
{
	EIMGArenaTileType Type = EIMGArenaTileType::EATT_Empty;
	TArray<FIMGCellData*> Cells;
	AWFCActor* WFCActor;

	void GetBoxAroundCluster(
		UObject* WorldContextObject,
		FVector& OutRectCenter,
		FRotator& OutRectRotation,
		float& OutSideLengthX,
		float& OutSideLengthY,
		bool bDebugDraw
	);

	bool IsPointInsideCluster(FVector Point);
};
#pragma endregion

USTRUCT(BlueprintType)
struct ISLANDMAPGENERATION_API FIMGMinMaxInt
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0))
	int32 Min = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0))
	int32 Max = 0;
};

USTRUCT(BlueprintType)
struct ISLANDMAPGENERATION_API FIMGExtrudeData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FVector2D> Vertices;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Height = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EIMGArenaTileType TileType;
};

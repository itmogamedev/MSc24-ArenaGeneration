// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Arena/IMGArenaGenerationHelper.h"

DEFINE_LOG_CATEGORY_STATIC(LogIMGArenaGenerationHelper, All, All)

EIMGArenaType FIMGArenaGenerationHelper::ChooseMacroPattern(TMap<TSubclassOf<UEnemy>, int32> EnemiesToSpawn,
                                                            TMap<TSubclassOf<UEnemy>, FIMGEnemyGenerationData>
                                                            Data)
{
	float CorridorScore = 0.f;
	float ArenaScore = 0.f;

	for (const auto EnemiesCount : EnemiesToSpawn)
	{
		const FIMGEnemyGenerationData* GenerationData = Data.Find(EnemiesCount.Key);
		if (!GenerationData)
		{
			UE_LOG(LogIMGArenaGenerationHelper, Error, TEXT("Generation data has no such enemy class!"));
		}

		const float PreferredWideness = GenerationData->PreferredArenaWideness;
		if (FMath::IsNearlyEqual(PreferredWideness, .5f)) { continue; }

		if (PreferredWideness < .5f) { CorridorScore += PreferredWideness * GenerationData->Efficiency; }
		else { ArenaScore += PreferredWideness * GenerationData->Efficiency; }
	}

	if (CorridorScore > ArenaScore) { return EIMGArenaType::EAT_Corridor; }
	return EIMGArenaType::EAT_Arena;
}

void FIMGArenaGenerationHelper::GetArenaTilesMaxNum(TMap<EIMGArenaTileType, int32>& TilesMaxNum, int32 TotalTilesNum,
                                                    const TMap<TSubclassOf<UEnemy>, int32>& EnemiesToSpawn,
                                                    const TMap<TSubclassOf<UEnemy>, FIMGEnemyGenerationData>&
                                                    Data,
                                                    const TMap<EIMGEnemyMovementType, FIMGTileTypeToMultiplier>&
                                                    MovementTypeToArenaTypeImpact)
{
	const float TotalEfficiency = GetTotalEnemiesSetEfficiency(EnemiesToSpawn, Data);
	const TArray ValuableTileTypes = {
		EIMGArenaTileType::EATT_Gallery,
		EIMGArenaTileType::EATT_AlternativePath,
		EIMGArenaTileType::EATT_ChokePoint,
		EIMGArenaTileType::EATT_SniperPosition
	};

	TMap<EIMGArenaTileType, float> ValuableTypeToEfficiency;

	for (const auto EnemiesCount : EnemiesToSpawn)
	{
		for (const EIMGArenaTileType Type : ValuableTileTypes)
		{
			const FIMGEnemyGenerationData* GenerationData = Data.Find(EnemiesCount.Key);
			if (!GenerationData)
			{
				UE_LOG(LogIMGArenaGenerationHelper, Error, TEXT("Generation data has no such enemy class!"));
			}
			float TotalMultiplier = 0.f;

			const float* MesoPatternMultiplier = GenerationData->MesoPatternEfficiencyImpact.Find(Type);
			if (MesoPatternMultiplier) { TotalMultiplier += *MesoPatternMultiplier; }

			const FIMGTileTypeToMultiplier* TypeToMultiplier = MovementTypeToArenaTypeImpact.Find(
				GenerationData->MovementType);
			if (TypeToMultiplier)
			{
				const float* MovementTypeMultiplier = TypeToMultiplier->MultiplierMap.Find(Type);
				if (MovementTypeMultiplier) { TotalMultiplier += *MovementTypeMultiplier; }
			}

			ValuableTypeToEfficiency.FindOrAdd(Type) +=
				TotalMultiplier * GenerationData->Efficiency * EnemiesCount.Value;
		}
	}

	int32 TotalTypedTiles = 0;
	for (auto TypesEfficiency : ValuableTypeToEfficiency)
	{
		const int32 TypeTilesMaxCount = FMath::RoundToInt32(TotalTilesNum * (TypesEfficiency.Value / TotalEfficiency));
		TilesMaxNum.FindOrAdd(TypesEfficiency.Key, TypeTilesMaxCount);
		TotalTypedTiles += TypeTilesMaxCount;
	}

	const float ReduceMultiplier = static_cast<float>(TotalTilesNum) / static_cast<float>(TotalTypedTiles);
	if (ReduceMultiplier >= 1.f) { return; }

	UE_LOG(LogIMGArenaGenerationHelper, Display, TEXT("Total tiles: %d"), TotalTilesNum);
	for (auto TypesEfficiency : ValuableTypeToEfficiency)
	{
		int32* TilesNum = TilesMaxNum.Find(TypesEfficiency.Key);
		if (!TilesNum) { continue; }

		int32 ReducedNum = FMath::RoundToInt32(static_cast<float>(*TilesNum) * ReduceMultiplier);
		*TilesNum = ReducedNum;
		UE_LOG(LogIMGArenaGenerationHelper, Display, TEXT("Type %d tiles: %d"), TypesEfficiency.Key, ReducedNum);
	}
}

float FIMGArenaGenerationHelper::GetTotalEnemiesSetEfficiency(
	const TMap<TSubclassOf<UEnemy>, int32>& EnemiesToSpawn,
	const TMap<TSubclassOf<UEnemy>, FIMGEnemyGenerationData>& Data)
{
	float TotalEfficiency = 0.f;

	for (const auto EnemiesCount : EnemiesToSpawn)
	{
		const FIMGEnemyGenerationData* GenerationData = Data.Find(EnemiesCount.Key);
		if (!GenerationData)
		{
			UE_LOG(LogIMGArenaGenerationHelper, Error, TEXT("Generation data has no such enemy class!"));
		}

		TotalEfficiency += GenerationData->Efficiency * EnemiesCount.Value;
	}

	return TotalEfficiency;
}

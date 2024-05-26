// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/IMGTypes.h"
#include "IMGDebugActor.generated.h"

class UTextRenderComponent;
class UStaticMeshComponent;

UCLASS()
class ISLANDMAPGENERATION_API AIMGDebugActor : public AActor
{
	GENERATED_BODY()

public:
	AIMGDebugActor();

	void SetStatusColor(EIMGGridCellStatus NewStatus);
	void SetDebugCoords(int32 X, int32 Y, int32 Z);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* DebugMesh;

	UPROPERTY(EditAnywhere)
	UTextRenderComponent* DebugText;

	UMaterialInterface* Material;
};

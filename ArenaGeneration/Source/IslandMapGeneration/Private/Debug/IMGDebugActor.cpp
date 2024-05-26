// Fill out your copyright notice in the Description page of Project Settings.


#include "Debug/IMGDebugActor.h"
#include "Components/TextRenderComponent.h"

AIMGDebugActor::AIMGDebugActor()
{
	PrimaryActorTick.bCanEverTick = false;

	DebugMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Debug Mesh"));
	SetRootComponent(DebugMesh);

	DebugText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Debug Text"));
	DebugText->SetupAttachment(GetRootComponent());
}

void AIMGDebugActor::BeginPlay()
{
	Super::BeginPlay();

	Material = DebugMesh->GetMaterial(0);
}

void AIMGDebugActor::SetStatusColor(EIMGGridCellStatus NewStatus)
{
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
	if (!DynamicMaterial) return;

	DebugMesh->SetMaterial(0, DynamicMaterial);

	switch (NewStatus)
	{
		case EIMGGridCellStatus::EGCS_Inactive:
			DynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor::Red);
			break;
		case EIMGGridCellStatus::EGCS_Active:
			DynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor::Green);
			break;
		case EIMGGridCellStatus::EGCS_Filled:
			DynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor::Blue);
			break;
		case EIMGGridCellStatus::EGCS_FailedToFill:
			DynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor::Yellow);
			break;
	}
}

void AIMGDebugActor::SetDebugCoords(int32 X, int32 Y, int32 Z)
{
	DebugText->SetText(FText::FromString(FString::Printf(TEXT("X:%d, Y:%d, Z:%d"), X, Y, Z)));
}

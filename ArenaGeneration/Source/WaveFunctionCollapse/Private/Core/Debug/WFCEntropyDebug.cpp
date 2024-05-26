// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Debug/WFCEntropyDebug.h"

#include "Components/TextRenderComponent.h"


// Sets default values
AWFCEntropyDebug::AWFCEntropyDebug()
{
	PrimaryActorTick.bCanEverTick = false;
	TextComponent = CreateDefaultSubobject<UTextRenderComponent>("Text");
	TextComponent->SetXScale(2.f);
	TextComponent->SetYScale(2.f);
}

void AWFCEntropyDebug::SetDebugText(FString Text) { TextComponent->SetText(FText::FromString(Text)); }

// Called when the game starts or when spawned
void AWFCEntropyDebug::BeginPlay() { Super::BeginPlay(); }

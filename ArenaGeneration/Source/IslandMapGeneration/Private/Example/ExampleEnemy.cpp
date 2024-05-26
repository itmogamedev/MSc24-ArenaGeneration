// Fill out your copyright notice in the Description page of Project Settings.


#include "Example/ExampleEnemy.h"


// Sets default values
AExampleEnemy::AExampleEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AExampleEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExampleEnemy::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

// Called to bind functionality to input
void AExampleEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "VRGameMode.h"


// Sets default values
AVRGameMode::AVRGameMode()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVRGameMode::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AVRGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
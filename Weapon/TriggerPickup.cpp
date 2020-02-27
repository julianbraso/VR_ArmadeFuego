// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerPickup.h"

// Sets default values
ATriggerPickup::ATriggerPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = root;
	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	mesh->SetupAttachment(root);
	mesh->SetCustomDepthStencilValue(253);

}

// Called when the game starts or when spawned
void ATriggerPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATriggerPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UStaticMeshComponent * ATriggerPickup::GetMesh()
{
	if (mesh)
	{
		return mesh;
	}
	return nullptr;
}

void ATriggerPickup::ReleaseFromHandle()
{
	if (grabbingHandle) {
		grabbingHandle->ReleaseComponent();
	}
}

void ATriggerPickup::OnPickup_Implementation(USceneComponent* hand)
{
	if (hand)
	{
		grabbingHand = hand;
	}
	bWasSimulating = mesh->IsSimulatingPhysics();
	if (bWasSimulating && !bUseHandle) {
		mesh->SetSimulatePhysics(false);
	}
}

void ATriggerPickup::OnRelease_Implementation(USceneComponent* hand)
{
	if (grabbingHand)
	{
		grabbingHand = nullptr;
	}
	if (bWasSimulating && !bUseHandle) {
		mesh->SetSimulatePhysics(true);
	}
}


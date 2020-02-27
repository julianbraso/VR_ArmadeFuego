// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupObject.h"

// Sets default values
APickupObject::APickupObject()
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
void APickupObject::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickupObject::OnPickup_Implementation(USceneComponent* hand) {
	mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
}

void APickupObject::OnRelease_Implementation(USceneComponent* hand) {
	mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

// Called every frame
void APickupObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UStaticMeshComponent * APickupObject::GetMesh()
{
	if (mesh) {
		return mesh;
	}
	return nullptr;
}


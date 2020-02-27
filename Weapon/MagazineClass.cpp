// Fill out your copyright notice in the Description page of Project Settings.

#include "MagazineClass.h"
#include "Components/BoxComponent.h"
#include "BulletPickupClass.h"

// Sets default values
AMagazineClass::AMagazineClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bullet1Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual Bullet 1"));
	bullet1Mesh->SetVisibility(false);
	bullet1Mesh->SetupAttachment(mesh);
	bullet2Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual Bullet 2"));
	bullet2Mesh->SetVisibility(false);
	bullet2Mesh->SetupAttachment(mesh);
	bulletTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Bullet Trigger"));
	bulletTrigger->SetupAttachment(mesh);
	bulletTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMagazineClass::OnOverlap);		///OnComponentHit.AddDynamic(this, &AMyFPSProjectile::OnHit);
}

// Called when the game starts or when spawned
void AMagazineClass::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMagazineClass::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (bLoaded || currentNumberOfBullets == maxNumberOfBullets) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, "loaded or max bullets", true);
		return;
	}

	if (OtherActor->IsA<ABulletPickupClass>()) {
		ABulletPickupClass* a = Cast<ABulletPickupClass>(OtherActor);
		if (a) {
			if (a->bulletCaliber != magBulletCaliber) {
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, "fuck off with that caliber", true);
				return;
			}
			if (!a->grabbingHand) {
				return;
			}
			a->OnRelease(a->grabbingHand);
		}
		else {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "fokin error!", true);
			return;
		}

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, "bullet entered mag!", true);
		AddVisualBullet();
		OtherActor->Destroy();
		currentNumberOfBullets += 1;
	}
	else {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, OtherActor->GetName(), true);
	}
}

void AMagazineClass::AddVisualBullet_Implementation()
{
	if (!bullet1Mesh->bVisible)
	{
		bullet1Mesh->SetVisibility(true);
	}
	else {
		bullet2Mesh->SetVisibility(true);
	}
}

void AMagazineClass::DeleteVisualBullet_Implementation()
{
	if (bullet2Mesh->bVisible)
	{
		bullet2Mesh->SetVisibility(false);
	}
	else {
		bullet1Mesh->SetVisibility(false);
	}
}

// Called every frame
void AMagazineClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMagazineClass::MagLoaded(bool loaded)
{
	bLoaded = loaded;
	if (loaded) {
		mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		bulletTrigger->SetGenerateOverlapEvents(false);
	}
	else {
		mesh->SetCollisionResponseToAllChannels(ECR_Block);
		mesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
		mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
		bulletTrigger->SetGenerateOverlapEvents(true);
	}
}

void AMagazineClass::SetMeshPhysicsEnabled_Implementation(bool enabled)
{
	mesh->SetSimulatePhysics(enabled);
}


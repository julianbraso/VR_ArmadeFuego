// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MyFPSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
//#include "MyFPSCharacter.cpp"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "UnrealNetwork.h"
#include "Runtime/Engine/Classes/GameFramework/ProjectileMovementComponent.h"
//#include "WeaponBaseClass.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Runtime/Engine/Classes/PhysicalMaterials/PhysicalMaterial.h"

//class AMyFPSCharacter;

void AMyFPSProjectile::BeginPlay()
{
	Super::BeginPlay();
	StartLoc = GetActorLocation();
}

void AMyFPSProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyFPSProjectile::OnParticleCol(const FName EventName, float EmitterTime, int ParticleTime, const FVector Location, const FVector Velocity, const FVector Direction, const FVector Normal, const FName BoneName, UPhysicalMaterial * PhysMat)
{
	FCollisionQueryParams RV_TraceParams(FName(""));
	FHitResult RV_Hit(ForceInit);
	FVector startTrace = Location;
	FVector endTrace = Direction;
	if (GetWorld()->LineTraceSingleByChannel(RV_Hit, startTrace, endTrace, ECC_Visibility, RV_TraceParams)) {

	}

	//
	UStaticMeshComponent * m = Cast<UStaticMeshComponent>(RV_Hit.Component);
	if (!m)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "problem casting to static mesh");
		return;
	}

	float size = FMath::RandRange(32.f, 64.f);

	UGameplayStatics::SpawnDecalAttached(BloodSplatDecal, FVector(1.f, size, size), m, "", Location, UKismetMathLibrary::MakeRotFromX(Normal) , EAttachLocation::KeepWorldPosition,30.f);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, RV_Hit.Component->GetName());
	//UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BloodSplatDecal, FVector(1.f, 32.0f, 32.0f), Location, Direction.Rotation(), 0.f);						//->SetFadeScreenSize(0.0001f)
	Destroy();
}

AMyFPSProjectile::AMyFPSProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	
	//CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AMyFPSProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	impactFx = CreateDefaultSubobject<UParticleSystem>(TEXT("ImpactFx"));
	//impactFx->DeactivateSystem();

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	//ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->bAutoActivate = true;
	// Die after 3 seconds by default
	InitialLifeSpan = 0.0f;

	bReplicates = true;
	bReplicateMovement = true;

	Tags.Add("Bullet");
}

void AMyFPSProjectile::SpawnFX_All_Implementation(UWorld * World, UParticleSystem * EmitterTemplate, const FTransform & SpawnTransform, bool bAutoDestroy) {
	SpawnFX(World,EmitterTemplate,SpawnTransform,bAutoDestroy);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, EmitterTemplate->GetName());
}
void AMyFPSProjectile::InitVelocity(const FVector & ShootDirection)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = ShootDirection * ProjectileSpeed;
	}
}

UParticleSystem * AMyFPSProjectile::GetFXFromSurface(FHitResult Hit)
{
	if (Hit.PhysMaterial == NULL)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Hit Phys type: NULL");
		return impactFx_Dust;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,"Hit Phys type: " + UEnum::GetValueAsString(TEXT("Engine.PhysicalSurface"),Hit.PhysMaterial->SurfaceType));
	if (Hit.PhysMaterial->SurfaceType == EPhysicalSurface::SurfaceType_Default)
	{
		return impactFx_Dust;
	}
	else if (Hit.PhysMaterial->SurfaceType == EPhysicalSurface::SurfaceType1)				/////wood
	{
		return impactFx_Wood;
	}
	else  if (Hit.PhysMaterial->SurfaceType == EPhysicalSurface::SurfaceType2)				/////concrete
	{
		return impactFx_Concrete;
	}
	else  if (Hit.PhysMaterial->SurfaceType == EPhysicalSurface::SurfaceType3)				/////glass
	{
		return impactFx_Glass;
	}
	else  if (Hit.PhysMaterial->SurfaceType == EPhysicalSurface::SurfaceType4)				////grass
	{
		return impactFx_Grass;
	}
	else  if (Hit.PhysMaterial->SurfaceType == EPhysicalSurface::SurfaceType5)				////water
	{
		return impactFx_Water;
	}
	else  if (Hit.PhysMaterial->SurfaceType == EPhysicalSurface::SurfaceType6)				////flesh
	{
		return impactFx_Flesh;
	}
	else  if (Hit.PhysMaterial->SurfaceType == EPhysicalSurface::SurfaceType7)				////dust
	{
		return impactFx_Dust;
	}
	return nullptr;
}

void AMyFPSProjectile::SpawnFX(UWorld * World, UParticleSystem * EmitterTemplate, const FTransform & SpawnTransform, bool bAutoDestroy) {
	//UGameplayStatics::SpawnEmitterAtLocation(World, EmitterTemplate, SpawnTransform, bAutoDestroy)->OnParticleCollide.AddDynamic(this, &AMyFPSProjectile::OnParticleCol);
	UGameplayStatics::SpawnDecalAtLocation(World, BulletHitDecal, FVector(2,4,4),SpawnTransform.GetLocation(),SpawnTransform.GetRotation().Rotator(),20.f)->SetFadeScreenSize(0.0001f);
	/*GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Multi rot: "+SpawnTransform.GetRotation().ToString());*/
}

void AMyFPSProjectile::ServerSpawnFX_Implementation(UWorld * World, UParticleSystem * EmitterTemplate, const FTransform & SpawnTransform, bool bAutoDestroy) {
	SpawnFX_All(World, EmitterTemplate, SpawnTransform, bAutoDestroy);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, EmitterTemplate->GetName());
}

bool AMyFPSProjectile::ServerSpawnFX_Validate(UWorld * World, UParticleSystem * EmitterTemplate, const FTransform & SpawnTransform, bool bAutoDestroy) {
	return true;
}

void AMyFPSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent->SetVisibility(false, true);
	FTransform T;
	if (HasAuthority()) {

		this->SetLifeSpan(5.0f);
		
		//AMyFPSCharacter* p = Cast<AMyFPSCharacter>(OtherActor);

		/*if (p)
		{
			if (p == GetOwner())
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Hitting yeself");
				return;
			}
			p->TakeDamage(ProjectileDamage, FDamageEvent(), Instigator->Controller, p);
			p->OnDmg_BoneHit = Hit.BoneName;
			p->OnHit_Client(Hit.BoneName);
				//TakeDamage(p->ProjectileDamage, FDamageEvent(), p->Instigator->Controller, p);
		}*/

		impactFx = GetFXFromSurface(Hit);
		EndLoc = Hit.Location;
		DistanceTraveled = FVector(EndLoc - StartLoc).Size();
		DistanceTraveledInM = DistanceTraveled / 100;

		/*FVector v;
		if (DistanceTraveledInM <= 10)
		{
			v = FVector(0.5,0.5,0.5);
		}
		else if (DistanceTraveledInM <= 30)
		{
			v = FVector(1.0f, 1.0f, 1.0f);
		}
		else if (DistanceTraveledInM <= 60)
		{
			v = FVector(2.0f, 2.0f, 2.0f);
		}
		else if (DistanceTraveledInM <= 100)
		{
			v = FVector(4.0f, 4.0f, 4.0f);
		}
		else if (DistanceTraveledInM > 100)
		{
			v = FVector(5.0f, 5.0f, 5.0f);
		}
		*/
		T = FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint, FVector(1.f,1.f,1.f));

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::SanitizeFloat(DistanceTraveledInM));
		//FTransform T = FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint);
		SpawnFX_All(GetWorld(), impactFx, T, true);
		/*GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Server: " + Hit.ImpactPoint.ToString());*/
	}
	else {
		if (Role == ROLE_AutonomousProxy) {
			//impactFx = GetFXFromSurface(Hit);
			//ServerSpawnFX(GetWorld(), impactFx, T, true);
			/*GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Client: "+Hit.ImpactPoint.ToString());*/
		}
		return;
	}
	
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 2.0f, GetActorLocation());

		Destroy();
	}
	
}
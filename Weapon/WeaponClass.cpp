// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponClass.h"
#include "MagazineClass.h"
#include "Runtime/Engine/Classes/GameFramework/ProjectileMovementComponent.h"

AWeaponClass::AWeaponClass() {
	slide = CreateDefaultSubobject<UMovableComponent>("Slide");
	slide->SetupAttachment(mesh);
	muzzle = CreateDefaultSubobject<USceneComponent>("Muzzle");
	muzzle->SetupAttachment(mesh);
	magTrigger = CreateDefaultSubobject<UBoxComponent>("MagTrigger");
	magTrigger->SetupAttachment(mesh);
	magTrigger->OnComponentBeginOverlap.AddDynamic(this, &AWeaponClass::OnOverlap);
	magTrigger->SetBoxExtent(FVector(0.5f, 1.4f, 0.5f), true);
	chamberTrigger = CreateDefaultSubobject<UBoxComponent>("Chamber Trigger");
	chamberTrigger->SetupAttachment(mesh);
	chamberTrigger->SetBoxExtent(FVector(0.5f, 1.4f, 0.5f), true);
	chamberTrigger->OnComponentBeginOverlap.AddDynamic(this, &AWeaponClass::OnOverlapChamber);
	magPos = CreateDefaultSubobject<USceneComponent>("Mag Position");
	magPos->SetupAttachment(mesh);
	chamberBulletPos = CreateDefaultSubobject<USceneComponent>("chamberBullet Position");
	chamberBulletPos->SetupAttachment(chamberTrigger);
	
	slide->StartLimitDelegate.AddDynamic(this, &AWeaponClass::SlideFront);
	slide->EndLimitDelegate.AddDynamic(this, &AWeaponClass::SlideBack);

}

void AWeaponClass::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponClass::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	if (currentMag) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Green, "already has mag!", true);
		return;
	}
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Green, OtherActor->GetName(), true);

	//if (OtherComp->IsA<AMagazineClass>()) {
		AMagazineClass* a = Cast<AMagazineClass>(OtherActor);
		if (a) {
			if (!magClass || a->GetClass() != magClass.Get()) {				//magClass->GetClass() != a->GetClass()
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "different mag, or no mag class...", true);
				return;
			}

			//do stuff here
			a->ReleaseFromHandle();
			a->SetMeshPhysicsEnabled(false);
			a->GetMesh()->AttachToComponent(magPos, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName());
			currentMag = a;
			a->MagLoaded(true);

		}
		else {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "fokin error!", true);
			return;
		}

	//}
}

void AWeaponClass::OnOverlapChamber(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

}

void AWeaponClass::SlideBack()
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "Slide back delegate called!", true);
	if (bFired)		//meaning it fired and isnt moving with hand
	{
		//eject bullet case!
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "ejecting bullet case", true);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CaseFx, chamberTrigger->GetComponentLocation(), chamberTrigger->GetComponentRotation(), FVector(1.5f), true);
	}
	else {
		if (chamberedRound) {
			chamberedRound->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			chamberedRound->SetActorEnableCollision(true);
			chamberedRound->GetMesh()->SetSimulatePhysics(true);
			chamberedRound->GetMesh()->AddImpulse(GetMesh()->GetUpVector() * 100.f);
			chamberedRound = nullptr;
		}
	}

	if (currentMag && currentMag->currentNumberOfBullets <= 0) {
		slide->bHoldMovement = true;
	}

	bSlideBack = true;

}

void AWeaponClass::SlideFront()
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "Slide front delegate called!", true);

	if (!bSlideBack) {
		return;
	}

	if (currentMag && currentMag->currentNumberOfBullets > 0) {
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ActorSpawnParams.Instigator = Instigator;
		ActorSpawnParams.Owner = GetOwner();
		//APickupObject* bullet;
		// spawn the projectile at the muzzle
		const FRotator SpawnRotation = chamberBulletPos->GetComponentRotation();
		const FVector SpawnLocation = chamberBulletPos->GetComponentLocation();
		FTransform T = FTransform(SpawnRotation, SpawnLocation, FVector(1, 1, 1));

		if (!currentMag->bulletClass) {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "no bullet class on mag", true);
			return;
		}
		if (chamberedRound) {
			chamberedRound->Destroy();
		}

		chamberedRound = GetWorld()->SpawnActor<ABulletPickupClass>(currentMag->bulletClass, T,ActorSpawnParams);
		if (chamberedRound) {
			chamberedRound->AttachToComponent(chamberBulletPos,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			chamberedRound->SetActorEnableCollision(false);
		}
		currentMag->currentNumberOfBullets -= 1;
		currentMag->DeleteVisualBullet();
	}

	bCanFire = true;
	bSlideBack = false;
	bFired = false;
}

void AWeaponClass::EjectMag(){
	if (currentMag)
	{
		currentMag->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		currentMag->SetMeshPhysicsEnabled(true);
		currentMag->MagLoaded(false);
		currentMag = nullptr;
	}
}

void AWeaponClass::HoldSlideAnimation(bool value) {
	slide->bHoldMovement = value;
}

void AWeaponClass::Fire_Implementation()
{

	if (!chamberedRound || !bCanFire) {
		return;
	}

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ActorSpawnParams.Instigator = Instigator;
	ActorSpawnParams.Owner = GetOwner();
	AMyFPSProjectile* proj;
	// spawn the projectile at the muzzle
	const FRotator SpawnRotation = muzzle->GetComponentRotation();
	const FVector SpawnLocation = muzzle->GetComponentLocation();
	FTransform T = FTransform(SpawnRotation, SpawnLocation, FVector(1, 1, 1));

	proj = GetWorld()->SpawnActorDeferred<AMyFPSProjectile>(ProjectileClass, T, GetOwner(), GetInstigator(), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding);
	if (proj)
	{
		//proj->SetOwner(this);
		proj->Instigator = GetInstigator();
		proj->GetProjectileMovement()->MaxSpeed = ProjectileVelocity * 1000;
		proj->GetProjectileMovement()->InitialSpeed = ProjectileVelocity * 1000;
		proj->ProjectileDamage = Damage;
		proj->SetActorScale3D(FVector(1, 1, 1));
		proj->SetOwner(GetOwner());
		proj->FinishSpawning(T);
		//currentMag->currentNumberOfBullets -= 1;
		chamberedRound->Destroy();
		bFired = true;
		T.SetScale3D(FVector(0.25f, 0.25f, 0.25f));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFx, T, true);
	}

	if (slide != NULL)
	{
		slide->Animate();
		bCanFire = false;
	}	

}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovableComponent.h"
#include "Components/BoxComponent.h"
#include "PickupObject.h"
#include "BulletPickupClass.h"
#include "MagazineClass.h"
#include "MyFPSProjectile.h"
#include "WeaponClass.generated.h"

UENUM(BlueprintType)
enum class EWeaponFireMode : uint8 {
	FM_Single UMETA(DisplayName = "Single"),
	FM_Auto UMETA(DisplayName = "Auto")
};

/**
 * 
 */
UCLASS()
class POLISIM_API AWeaponClass : public APickupObject
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AWeaponClass();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UMovableComponent* slide;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* muzzle;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* magTrigger;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* chamberTrigger;

	UPROPERTY(VisibleAnywhere)
	ABulletPickupClass* chamberedRound;

	bool bSlideBack;

	UPROPERTY(EditAnywhere)
		class UParticleSystem* CaseFx;

	UPROPERTY(EditAnywhere)
		class UParticleSystem* MuzzleFx;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Damage;
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<class AMyFPSProjectile> ProjectileClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ProjectileVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float FireRate;
	UPROPERTY(BlueprintReadOnly)
		int32 CurrentAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
		int32 MaxMagAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo)
		int32 DefaultMagAmmo;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMagazineClass> magClass;

	UPROPERTY(EditAnywhere)
		USceneComponent* magPos;

	UPROPERTY(EditAnywhere)
		USceneComponent* chamberBulletPos;

	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapChamber(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
		AMagazineClass* currentMag;

	UFUNCTION()
		void SlideBack();

	UFUNCTION()
		void SlideFront();


	bool bCanFire;
	bool bFired;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintNativeEvent)
		void Fire();
	UFUNCTION()
		void HoldSlideAnimation(bool value);
	UFUNCTION()
		void EjectMag();
};

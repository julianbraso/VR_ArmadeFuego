// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "TriggerPickup.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "MagazineClass.generated.h"

UCLASS()
class POLISIM_API AMagazineClass : public ATriggerPickup
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMagazineClass();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
		EBulletCaliber magBulletCaliber;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* bullet1Mesh;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* bullet2Mesh;
	
	UPROPERTY(EditAnywhere)
		int32 maxNumberOfBullets;
	UPROPERTY(EditAnywhere)
		UBoxComponent* bulletTrigger;
	UPROPERTY()
		bool bLoaded;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent)
	void AddVisualBullet();

	
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintNativeEvent)
		void SetMeshPhysicsEnabled(bool enabled);
	UFUNCTION()
		void MagLoaded(bool loaded);
	UPROPERTY(EditAnywhere)
		int32 currentNumberOfBullets;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ABulletPickupClass> bulletClass;

	UFUNCTION(BlueprintNativeEvent)
		void DeleteVisualBullet();


};

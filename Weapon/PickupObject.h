// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupObject.generated.h"

UCLASS()
class POLISIM_API APickupObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
		USceneComponent* root;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* mesh;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent)
	void OnPickup(USceneComponent* hand);
	UFUNCTION(BlueprintNativeEvent)
	void OnRelease(USceneComponent* hand);

	UFUNCTION()
		UStaticMeshComponent* GetMesh();

};

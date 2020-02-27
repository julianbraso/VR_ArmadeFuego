// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "TriggerPickup.generated.h"

UCLASS()
class POLISIM_API ATriggerPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATriggerPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere)
		USceneComponent* root;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* mesh;
	
	bool bWasSimulating;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		FName socketForGrab;

	UFUNCTION(BlueprintNativeEvent)
	void OnPickup(USceneComponent* hand);

	UFUNCTION(BlueprintNativeEvent)
	void OnRelease(USceneComponent* hand);

	UFUNCTION()
		UStaticMeshComponent* GetMesh();

	UPROPERTY(EditAnywhere)
		bool bUseHandle;

	UFUNCTION()
		void ReleaseFromHandle();

	UPROPERTY()
		UPhysicsHandleComponent* grabbingHandle;

	UPROPERTY()
		USceneComponent* grabbingHand;

};

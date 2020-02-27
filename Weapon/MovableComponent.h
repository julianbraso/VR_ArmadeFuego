// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "MovableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndLimitDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartLimitDelegate);

UCLASS(meta = (BlueprintSpawnableComponent))
class POLISIM_API UMovableComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

		

public:	
	// Sets default values for this actor's properties
	UMovableComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/*UPROPERTY(VisibleAnywhere)
		USceneComponent* root;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* mesh;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* end;*/
	USceneComponent* grabbingHand;
	bool bisGrabbing;
	UPROPERTY(EditAnywhere)
		float fSlideBackSpeed = 10.0f;
	UPROPERTY(EditAnywhere)
		float fSlideBackLimitX = -5.0f;

	USceneComponent* root;

	float originalX;

	UPROPERTY(EditAnywhere)
	bool bUsingY = false;

	UPROPERTY()
	float grabPos;

	bool _a;
	bool _b;
	bool _c;
	bool _d;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintNativeEvent)
		void GrabbedEvent(USceneComponent* hand);
	UFUNCTION(BlueprintNativeEvent)
		void ReleasedEvent(USceneComponent* hand);
	UFUNCTION()
	void Animate();
	UPROPERTY()
		bool bHoldMovement;

	UPROPERTY(BlueprintAssignable)
		FEndLimitDelegate EndLimitDelegate;

	UPROPERTY(BlueprintAssignable)
		FStartLimitDelegate StartLimitDelegate;

};

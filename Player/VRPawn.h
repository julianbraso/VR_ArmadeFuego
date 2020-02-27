// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PickupObject.h"
#include "TriggerPickup.h"
#include "WeaponClass.h"
#include "MovableComponent.h"
#include "HeadMountedDisplay.h"
#include "SteamVRChaperoneComponent.h"
#include "Runtime/Engine/Classes/Components/SceneComponent.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h"
#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "VRPawn.generated.h"

UCLASS()
class POLISIM_API AVRPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVRPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void CreateComponents();
	void CreateHandController(USceneComponent* a_compParent, FName a_strDisplayName, FName a_nameHandType);
	USkeletalMeshComponent* CreateHandMesh(UMotionControllerComponent* a_compParent, FName a_strDisplayName, FName a_nameHandType);

	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void ObjectGrabbedEvent(int32 hand);
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void ObjectReleasedEvent(int32 hand);

	APickupObject* currentGrabbedObjectLeft;
	APickupObject* currentGrabbedObjectRight;

	UPROPERTY()
		AWeaponClass* currentWeaponLeft;
	UPROPERTY()
		AWeaponClass* currentWeaponRight;

	//UFUNCTION()
	void AttachToHand(int32 hand, USceneComponent* object, bool bSnap ,FName socketToAttach);

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UPhysicsHandleComponent* PhysicsHandleLeft;

	bool bLeftTriggerPressed;
	bool bRightTriggerPressed;

	UPROPERTY(VisibleAnywhere)
		USceneComponent* handleTargetLeft;

	UPROPERTY(VisibleAnywhere)
		USceneComponent* handleTargetRight;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UPhysicsHandleComponent* PhysicsHandleRight;

	UPROPERTY(VisibleAnywhere)
		USceneComponent* rootComp;
	UPROPERTY(VisibleAnywhere)
		USceneComponent* compVRCameraRoot;
	UPROPERTY(VisibleAnywhere)
		USteamVRChaperoneComponent* chaperone;
	UPROPERTY(VisibleAnywhere)
		UCameraComponent* compVRCamera;

	UPROPERTY(VisibleAnywhere)
	UMotionControllerComponent* leftHand;
	UPROPERTY(VisibleAnywhere)
	UMotionControllerComponent* rightHand;
	UPROPERTY(BlueprintReadOnly,VisibleAnywhere)
		USkeletalMeshComponent* leftHandMesh;
	UPROPERTY(VisibleAnywhere)
		USphereComponent* leftHandMeshGrabCollider;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		USkeletalMeshComponent* rightHandMesh;
	UPROPERTY(VisibleAnywhere)
		USphereComponent* rightHandMeshGrabCollider;
	UFUNCTION()
	void OnOverlapBeginLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapBeginRight(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEndRight(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
		void OnOverlapEndLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// UPrimitiveComponent*, OverlappedComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, int32, OtherBodyIndex, bool, bFromSweep, const FHitResult &, SweepResult);

	APickupObject* CheckObjectsNearHand(int32 hand);
	UMovableComponent* CheckForInteractables(int32 hand);
	UMovableComponent* lastGrabbedComponentLeft;
	UMovableComponent* lastGrabbedComponentRight;

	UFUNCTION(BlueprintNativeEvent, Category = "Input")
		void GripLeftHand_Pressed();
	UFUNCTION(BlueprintNativeEvent, Category = "Input")
		void GripRightHand_Pressed();
	UFUNCTION(BlueprintNativeEvent, Category = "Input")
		void GripLeftHand_Released();
	UFUNCTION(BlueprintNativeEvent, Category = "Input")
		void GripRightHand_Released();

	UFUNCTION(BlueprintNativeEvent, Category = "Input")
		void TriggerLeftHand_Pressed();
	UFUNCTION(BlueprintNativeEvent, Category = "Input")
		void TriggerRightHand_Pressed();
	UFUNCTION(BlueprintNativeEvent, Category = "Input")
		void TriggerLeftHand_Released();
	UFUNCTION(BlueprintNativeEvent, Category = "Input")
		void TriggerRightHand_Released();
	
	UFUNCTION()
	ATriggerPickup* CheckTriggerPickupNearHand(int32 hand);


	UFUNCTION(BlueprintNativeEvent, Category = "Input")
		void LeftDPadPressed_Left();
	UFUNCTION(BlueprintNativeEvent, Category = "Input")
		void LeftDPadPressed_Right();

	UFUNCTION(BlueprintNativeEvent, Category = "Input")
		void RightDPadPressed_Left();
	UFUNCTION(BlueprintNativeEvent, Category = "Input")
		void RightDPadPressed_Right();
	
	ATriggerPickup* grabbedTriggerPickupLeft;
	
	ATriggerPickup* grabbedTriggerPickupRight;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

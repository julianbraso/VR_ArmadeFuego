// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPawn.h"
/* VR Includes */
#include "HeadMountedDisplay.h"
#include "MotionControllerComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "PickupObject.h"
#include "TriggerPickup.h"
#include "WeaponClass.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Haptics/HapticFeedbackEffect_Base.h"
#include "Engine.h"

// Sets default values
AVRPawn::AVRPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CreateComponents();
}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Player Pawn Base Begin Play", true);
	//UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
}

void AVRPawn::CreateComponents()
{
	//The default root component for the pawn.
	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = rootComp;

	//Component that will be used for the movement based on input
	//UFloatingPawnMovement* compFloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingMovementComp"));

	//Chaperone component that is required for Steam VR plugin
	chaperone = CreateDefaultSubobject<USteamVRChaperoneComponent>(TEXT("SteamVR Chaperone"));

	//Create a scene component that will act as the parent for the camera. This might come in handy later.
	compVRCameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VR Camera Root"));
	compVRCameraRoot->SetupAttachment(rootComp);
	compVRCameraRoot->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
	compVRCameraRoot->SetRelativeScale3D(FVector::OneVector);

	//Create a camera component and attach this to the camera root.
	compVRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("HMD Camera"));
	compVRCamera->SetupAttachment(compVRCameraRoot);
	compVRCamera->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
	compVRCamera->SetRelativeScale3D(FVector::OneVector);

	//left hand controller!
	leftHand = CreateDefaultSubobject<UMotionControllerComponent>("MC_Left", true);
	leftHand->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
	leftHand->SetRelativeScale3D(FVector::OneVector);

	leftHand->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
	leftHand->SetupAttachment(compVRCameraRoot);

	//Create the hand mesh for visualization
	//CreateHandMesh(compMotionController, strMeshDisplayName, a_nameHandType);
	//create the mesh component
	leftHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>("LeftHandMesh");
	//set the mesh to the component
	//leftHandMesh->SetSkeletalMesh(HandMeshObject.Object, true);

	//Set the defaults
	leftHandMesh->SetAutoActivate(true);
	leftHandMesh->SetVisibility(true);
	leftHandMesh->SetHiddenInGame(false);
	FQuat qRotation = FQuat::Identity;
	FVector vec3Scale = FVector::OneVector;

		qRotation = FQuat(FVector(1, 0, 0), FMath::DegreesToRadians(90));
		vec3Scale = FVector(1, -1, 1);
		
		leftHandMesh->SetRelativeLocationAndRotation(FVector::ZeroVector, qRotation);
		leftHandMesh->SetRelativeScale3D(vec3Scale);
		leftHandMesh->SetupAttachment(leftHand);
		leftHandMeshGrabCollider = CreateDefaultSubobject<USphereComponent>("LeftHandGrabCollider");
		leftHandMeshGrabCollider->SetupAttachment(leftHandMesh);
		leftHandMeshGrabCollider->OnComponentBeginOverlap.AddDynamic(this, &AVRPawn::OnOverlapBeginLeft);
		leftHandMeshGrabCollider->OnComponentEndOverlap.AddDynamic(this, &AVRPawn::OnOverlapEndLeft);

		PhysicsHandleLeft = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandleLeft"));

		handleTargetLeft = CreateDefaultSubobject<USceneComponent>("LeftHandle Target");
		handleTargetLeft->SetupAttachment(leftHandMesh);

		//right hand controller!
		rightHand = CreateDefaultSubobject<UMotionControllerComponent>("MC_Right", true);
		rightHand->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
		rightHand->SetRelativeScale3D(FVector::OneVector);

		rightHand->MotionSource = FXRMotionControllerBase::RightHandSourceId;
		rightHand->SetupAttachment(compVRCameraRoot);

		//Create the hand mesh for visualization
		//CreateHandMesh(compMotionController, strMeshDisplayName, a_nameHandType);
		//create the mesh component
		rightHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>("RightHandMesh");
		//set the mesh to the component
		//leftHandMesh->SetSkeletalMesh(HandMeshObject.Object, true);

		//Set the defaults
		rightHandMesh->SetAutoActivate(true);
		rightHandMesh->SetVisibility(true);
		rightHandMesh->SetHiddenInGame(false);
		qRotation = FQuat::Identity;
		vec3Scale = FVector::OneVector;
		qRotation = FQuat(FVector(1, 0, 0), FMath::DegreesToRadians(270));
		rightHandMesh->SetRelativeLocationAndRotation(FVector::ZeroVector, qRotation);
		rightHandMesh->SetRelativeScale3D(vec3Scale);
		rightHandMesh->SetupAttachment(rightHand);
		rightHandMeshGrabCollider = CreateDefaultSubobject<USphereComponent>("RightHandGrabCollider");
		rightHandMeshGrabCollider->SetupAttachment(rightHandMesh);
		rightHandMeshGrabCollider->OnComponentBeginOverlap.AddDynamic(this, &AVRPawn::OnOverlapBeginRight);
		rightHandMeshGrabCollider->OnComponentEndOverlap.AddDynamic(this, &AVRPawn::OnOverlapEndRight);

		PhysicsHandleRight = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandleRight"));
		PhysicsHandleRight->bRotationConstrained = true;
		PhysicsHandleLeft->bRotationConstrained = true;

		handleTargetRight = CreateDefaultSubobject<USceneComponent>("RightHandle Target");
		handleTargetRight->SetupAttachment(rightHandMesh);
		//PhysicsHandleRight->location;

}



void AVRPawn::GripLeftHand_Pressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Left Hand Grip Pressed"));

	if (currentGrabbedObjectLeft) {
		currentGrabbedObjectLeft->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		currentGrabbedObjectLeft->OnRelease(leftHandMesh);
		currentGrabbedObjectLeft = nullptr;
		if (currentWeaponLeft) {
			currentWeaponLeft = nullptr;
		}
		ObjectReleasedEvent(0);
		return;
	}

	currentGrabbedObjectLeft = CheckObjectsNearHand(0);
	if (currentGrabbedObjectLeft) {
		AttachToHand(0, currentGrabbedObjectLeft->GetRootComponent(), true , FName("gunSocket"));
		currentGrabbedObjectLeft->OnPickup(leftHandMesh);
		//ObjectGrabbedEvent(0);
		if (currentGrabbedObjectLeft->IsA<AWeaponClass>()) {
			currentWeaponLeft = Cast<AWeaponClass>(currentGrabbedObjectLeft);
		}
	}
}
void AVRPawn::GripRightHand_Pressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Right Hand Grip Pressed"));

	if (currentGrabbedObjectRight) {
		currentGrabbedObjectRight->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		currentGrabbedObjectRight->OnRelease(rightHandMesh);
		currentGrabbedObjectRight = nullptr;
		if (currentWeaponRight) {
			currentWeaponRight = nullptr;
		}
		ObjectReleasedEvent(1);
		return;
	}

	currentGrabbedObjectRight = CheckObjectsNearHand(1);
	if (currentGrabbedObjectRight) {
		AttachToHand(1, currentGrabbedObjectRight->GetRootComponent(), true, FName("gunSocket"));
		currentGrabbedObjectRight->OnPickup(rightHandMesh);
		//ObjectGrabbedEvent(1);
		if (currentGrabbedObjectRight->IsA<AWeaponClass>()) {
			currentWeaponRight = Cast<AWeaponClass>(currentGrabbedObjectRight);
		}
	}
}
void AVRPawn::GripLeftHand_Released_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Left Hand Grip Released"));
}
void AVRPawn::GripRightHand_Released_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Right Hand Grip Released"));
}

void AVRPawn::TriggerLeftHand_Pressed_Implementation() {
	UE_LOG(LogTemp, Log, TEXT("Left Hand Trigger Pressed"));

	if (currentWeaponLeft) {
		currentWeaponLeft->Fire();
		return;
	}

		//lastGrabbedComponentLeft = CheckForInteractables(0);
		if (lastGrabbedComponentLeft != nullptr) {
			lastGrabbedComponentLeft->GrabbedEvent(leftHandMeshGrabCollider);
			return;
		}
		else {
			UE_LOG(LogTemp, Log, TEXT("Left Hand Trigger: no interactable!"));
		}

		grabbedTriggerPickupLeft = CheckTriggerPickupNearHand(0);

		if (grabbedTriggerPickupLeft) {
			grabbedTriggerPickupLeft->OnPickup(leftHandMesh);
			grabbedTriggerPickupLeft->SetOwner(this);
			grabbedTriggerPickupLeft->grabbingHandle = PhysicsHandleLeft;
			if (!grabbedTriggerPickupLeft->bUseHandle) {
				AttachToHand(0, grabbedTriggerPickupLeft->GetMesh(), false, grabbedTriggerPickupLeft->socketForGrab);
			}
			else {
				grabbedTriggerPickupLeft->GetMesh()->SetWorldLocationAndRotation(handleTargetLeft->GetComponentLocation(), handleTargetLeft->GetComponentRotation());
				grabbedTriggerPickupLeft->GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
				PhysicsHandleLeft->GrabComponentAtLocationWithRotation(grabbedTriggerPickupLeft->GetMesh(), FName(), handleTargetLeft->GetComponentLocation(), handleTargetLeft->GetComponentRotation());
			}
			
		}
		bLeftTriggerPressed = true;
}

void AVRPawn::TriggerLeftHand_Released_Implementation() {
	UE_LOG(LogTemp, Log, TEXT("Left Hand Trigger Released"));
	if (lastGrabbedComponentLeft != nullptr) {
		lastGrabbedComponentLeft->ReleasedEvent(leftHandMeshGrabCollider);
		lastGrabbedComponentLeft = nullptr;
		return;
	}

	if (grabbedTriggerPickupLeft) {
		if (!grabbedTriggerPickupLeft->bUseHandle) {
			grabbedTriggerPickupLeft->GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);			//->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}
		else {
			if (PhysicsHandleLeft->GetGrabbedComponent() != NULL) {
				PhysicsHandleLeft->ReleaseComponent();
				grabbedTriggerPickupLeft->GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
			}
			
		}
		grabbedTriggerPickupLeft->OnRelease(leftHandMesh);
		grabbedTriggerPickupLeft->SetOwner(NULL);
		grabbedTriggerPickupLeft->grabbingHandle = nullptr;
		grabbedTriggerPickupLeft = nullptr;
		return;
	}
	bLeftTriggerPressed = false;
	
}

void AVRPawn::TriggerRightHand_Pressed_Implementation() {
	UE_LOG(LogTemp, Log, TEXT("Right Hand Trigger Pressed"));

	if (currentWeaponRight) {
		currentWeaponRight->Fire();
		return;
	}

	//lastGrabbedComponentRight = CheckForInteractables(1);
	if (lastGrabbedComponentRight != nullptr) {
		lastGrabbedComponentRight->GrabbedEvent(rightHandMeshGrabCollider);
		return;
	} else{
		UE_LOG(LogTemp, Log, TEXT("Right Hand Trigger: no interactable!"));
	}

	grabbedTriggerPickupRight = CheckTriggerPickupNearHand(1);

	if (grabbedTriggerPickupRight) {
		grabbedTriggerPickupRight->OnPickup(rightHandMesh);
		grabbedTriggerPickupRight->grabbingHandle = PhysicsHandleRight;
		grabbedTriggerPickupRight->SetOwner(this);
		if (!grabbedTriggerPickupRight->bUseHandle) {
			AttachToHand(1, grabbedTriggerPickupRight->GetMesh(), false, grabbedTriggerPickupRight->socketForGrab);
		}
		else {
			//attach to physics handle on right hand!
			grabbedTriggerPickupRight->GetMesh()->SetWorldLocationAndRotation(handleTargetRight->GetComponentLocation(), handleTargetRight->GetComponentRotation());
			grabbedTriggerPickupRight->GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
			PhysicsHandleRight->GrabComponentAtLocationWithRotation(grabbedTriggerPickupRight->GetMesh(), FName(), handleTargetRight->GetComponentLocation(), handleTargetRight->GetComponentRotation());
		}
	}
	bRightTriggerPressed = true;
}

void AVRPawn::TriggerRightHand_Released_Implementation() {
	UE_LOG(LogTemp, Log, TEXT("Right Hand Trigger Released"));
	if (lastGrabbedComponentRight != nullptr) {
		lastGrabbedComponentRight->ReleasedEvent(rightHandMeshGrabCollider);
		lastGrabbedComponentRight = nullptr;
		return;
	}

	if (grabbedTriggerPickupRight) {
		if (!grabbedTriggerPickupRight->bUseHandle) {
			grabbedTriggerPickupRight->GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}
		else {
			if (PhysicsHandleRight->GetGrabbedComponent() != NULL) {
				PhysicsHandleRight->ReleaseComponent();
				grabbedTriggerPickupRight->GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
			}
		}
		grabbedTriggerPickupRight->OnRelease(rightHandMesh);
		grabbedTriggerPickupRight->SetOwner(NULL);
		grabbedTriggerPickupRight->grabbingHandle = nullptr;
		grabbedTriggerPickupRight = nullptr;
	}
	bRightTriggerPressed = false;
	
}

void AVRPawn::AttachToHand(int32 hand, USceneComponent* object, bool bSnap ,FName socketToAttach) {

	FAttachmentTransformRules a = FAttachmentTransformRules::KeepWorldTransform;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, socketToAttach.ToString(), true);

	if (bSnap) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Snapping to target!", true);
		a = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
	}
	else {
		a = FAttachmentTransformRules::KeepWorldTransform;
		if (socketToAttach != NAME_None) {
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "this has a socket name... snapping to target!", true);
			a = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
		}
	}

	if (hand == 0) {
		object->AttachToComponent(leftHandMesh,a,socketToAttach );
		
	}
	else if (hand == 1) {
		object->AttachToComponent(rightHandMesh, a, socketToAttach);
	}
	ObjectGrabbedEvent(hand);
}

void AVRPawn::OnOverlapBeginLeft(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, OtherComp->GetName(), true);
		if (OtherComp->IsA<UMovableComponent>()) {
			lastGrabbedComponentLeft = Cast<UMovableComponent>(OtherComp);
			lastGrabbedComponentLeft->SetCustomDepthStencilValue(253);
			lastGrabbedComponentLeft->SetRenderCustomDepth(true);
		}
		if (OtherActor->IsA<APickupObject>()) {
			APickupObject* a = Cast<APickupObject>(OtherActor);
			if (a) {
				a->GetMesh()->SetCustomDepthStencilValue(253);
				a->GetMesh()->SetRenderCustomDepth(true);
			}
		}
		if (OtherActor->IsA<ATriggerPickup>()) {
			ATriggerPickup* b = Cast<ATriggerPickup>(OtherActor);
			if (b) {
				b->GetMesh()->SetCustomDepthStencilValue(253);
				b->GetMesh()->SetRenderCustomDepth(true);
			}
		}
}

void AVRPawn::OnOverlapBeginRight(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, OtherComp->GetName(), true);
	if (OtherComp->IsA<UMovableComponent>()) {
		lastGrabbedComponentRight = Cast<UMovableComponent>(OtherComp);
		lastGrabbedComponentRight->SetCustomDepthStencilValue(253);
		lastGrabbedComponentRight->SetRenderCustomDepth(true);
	}
	if (OtherActor->IsA<APickupObject>()) {
		APickupObject* a = Cast<APickupObject>(OtherActor);
		if (a) {
			a->GetMesh()->SetCustomDepthStencilValue(253);
			a->GetMesh()->SetRenderCustomDepth(true);
		}
	}
	if (OtherActor->IsA<ATriggerPickup>()) {
		ATriggerPickup* b = Cast<ATriggerPickup>(OtherActor);
		if (b) {
			b->GetMesh()->SetCustomDepthStencilValue(253);
			b->GetMesh()->SetRenderCustomDepth(true);
		}
	}
}

void AVRPawn::OnOverlapEndRight(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp->IsA<UMovableComponent>()) {
		UMovableComponent* b = Cast<UMovableComponent>(OtherComp);
		b->SetRenderCustomDepth(false);
	}
	if (OtherActor->IsA<APickupObject>()) {
		APickupObject* a = Cast<APickupObject>(OtherActor);
		if (a) {
			a->GetMesh()->SetRenderCustomDepth(false);
		}
	}
	if (OtherActor->IsA<ATriggerPickup>()) {
		ATriggerPickup* b = Cast<ATriggerPickup>(OtherActor);
		if (b) {
			b->GetMesh()->SetRenderCustomDepth(false);
		}
	}
}

void AVRPawn::OnOverlapEndLeft(UPrimitiveComponent * OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp->IsA<UMovableComponent>()) {
		UMovableComponent* b = Cast<UMovableComponent>(OtherComp);
		b->SetRenderCustomDepth(false);
	}
	if (OtherActor->IsA<APickupObject>()) {
		APickupObject* a = Cast<APickupObject>(OtherActor);
		if (a) {
			a->GetMesh()->SetRenderCustomDepth(false);
		}
	}
	if (OtherActor->IsA<ATriggerPickup>()) {
		ATriggerPickup* b = Cast<ATriggerPickup>(OtherActor);
		if (b) {
			b->GetMesh()->SetRenderCustomDepth(false);
		}
	}
}

APickupObject* AVRPawn::CheckObjectsNearHand(int32 hand)
{
	// 0 = left, 1 = right
	if (hand == 0) {
		TSet<AActor*> a;
		leftHandMeshGrabCollider->GetOverlappingActors(a);
		if (a.Num() > 0) {
			for (AActor* actor : a)
			{
				if (actor->IsA<APickupObject>()) {
					if (GEngine)
						GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, "Found a pickup object on hand! ", true);
					APickupObject* pO = Cast<APickupObject>(actor);
					if (pO)
					{
						return pO;
					}
					else {
						if (GEngine)
							GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "ERROR ON CAST to PO", true);
						return nullptr;
					}
					 
				}
			}
		}
	}
	else if (hand == 1) {
		TSet<AActor*> a;
		rightHandMeshGrabCollider->GetOverlappingActors(a);
		if (a.Num() > 0) {
			for (AActor* actor : a)
			{
				if (actor->IsA<APickupObject>()) {
					if (GEngine)
						GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, "Found a pickup object on hand!", true);
					APickupObject* pO = Cast<APickupObject>(actor);
					if (pO)
					{
						return pO;
					}
					else {
						if (GEngine)
							GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "ERROR ON CAST to PO", true);
						return nullptr;
					}

				}
			}

		}
	}
	else {
		return nullptr;
	}
	return nullptr;
}

ATriggerPickup* AVRPawn::CheckTriggerPickupNearHand(int32 hand)
{
	// 0 = left, 1 = right
	if (hand == 0) {
		TSet<AActor*> a;
		leftHandMeshGrabCollider->GetOverlappingActors(a);
		if (a.Num() > 0) {
			for (AActor* actor : a)
			{
				if (actor->IsA<ATriggerPickup>()) {
					if (GEngine)
						GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, "Found a trigger pickup on hand! ", true);
					ATriggerPickup* pO = Cast<ATriggerPickup>(actor);
					if (pO)
					{
						return pO;
					}
					else {
						if (GEngine)
							GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "ERROR ON CAST to PO", true);
						return nullptr;
					}

				}
			}
		}
	}
	else if (hand == 1) {
		TSet<AActor*> a;
		rightHandMeshGrabCollider->GetOverlappingActors(a);
		if (a.Num() > 0) {
			for (AActor* actor : a)
			{
				if (actor->IsA<ATriggerPickup>()) {
					if (GEngine)
						GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, "Found a trigger pickup on hand!", true);
					ATriggerPickup* pO = Cast<ATriggerPickup>(actor);
					if (pO)
					{
						return pO;
					}
					else {
						if (GEngine)
							GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "ERROR ON CAST to PO", true);
						return nullptr;
					}

				}
			}

		}
	}
	else {
		return nullptr;
	}
	return nullptr;
}

void AVRPawn::LeftDPadPressed_Left_Implementation()
{
	if (currentWeaponLeft) {
		currentWeaponLeft->HoldSlideAnimation(false);
	}
}

void AVRPawn::LeftDPadPressed_Right_Implementation()
{
	if (currentWeaponRight) {
		currentWeaponRight->HoldSlideAnimation(false);
	}
}

void AVRPawn::RightDPadPressed_Left_Implementation()
{
	if (currentWeaponLeft) {
		currentWeaponLeft->EjectMag();
	}
}

void AVRPawn::RightDPadPressed_Right_Implementation()
{
	if (currentWeaponRight) {
		currentWeaponRight->EjectMag();
	}
}

UMovableComponent * AVRPawn::CheckForInteractables(int32 hand)
{

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	//RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);

	FVector start;
	FVector end;

	if (hand == 0) {
		start = leftHandMeshGrabCollider->GetComponentLocation();
		end = start + (leftHandMeshGrabCollider->GetForwardVector() * 5.f);
	}
	else if (hand == 1) {
		start = rightHandMeshGrabCollider->GetComponentLocation();
		end = start + (rightHandMeshGrabCollider->GetForwardVector() * 5.f);
	}
	

	//call GetWorld() from within an actor extending class
	
	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,        //result
		start,    //start
		end, //end
		ECC_Pawn, //collision channel
		RV_TraceParams
	);

	DrawDebugLine(
		GetWorld(),
		start,
		end,
		FColor(255, 0, 0),
		false, 5, 0,
		0.2
	);

	if (RV_Hit.Component != NULL) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, RV_Hit.Component->GetName(), true);
		if (RV_Hit.Component->IsA<UMovableComponent>())
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, "Found a movable component on hand!", true);
			UMovableComponent* mC = Cast<UMovableComponent>(RV_Hit.Component);
			if (mC)
			{
				return mC;
			}
			else {
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "ERROR ON CAST to MC", true);
				return nullptr;
			}
		}
	}
	return nullptr;
}

// Called every frame
void AVRPawn::Tick(float DeltaTime)
{
	if (bLeftTriggerPressed) {
		PhysicsHandleLeft->SetTargetLocationAndRotation(handleTargetLeft->GetComponentLocation(), handleTargetLeft->GetComponentRotation());
	}
	if (bRightTriggerPressed) {
		PhysicsHandleRight->SetTargetLocationAndRotation(handleTargetRight->GetComponentLocation(), handleTargetRight->GetComponentRotation());
	}
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAction("GripLeft", IE_Pressed, this, &AVRPawn::GripLeftHand_Pressed);
	InputComponent->BindAction("GripRight", IE_Pressed, this, &AVRPawn::GripRightHand_Pressed);
	InputComponent->BindAction("GripLeft", IE_Released, this, &AVRPawn::GripLeftHand_Released);
	InputComponent->BindAction("GripRight", IE_Released, this, &AVRPawn::GripRightHand_Released);
	InputComponent->BindAction("TriggerLeft", IE_Pressed, this, &AVRPawn::TriggerLeftHand_Pressed);
	InputComponent->BindAction("TriggerRight", IE_Pressed, this, &AVRPawn::TriggerRightHand_Pressed);
	InputComponent->BindAction("TriggerLeft", IE_Released, this, &AVRPawn::TriggerLeftHand_Released);
	InputComponent->BindAction("TriggerRight", IE_Released, this, &AVRPawn::TriggerRightHand_Released);
	InputComponent->BindAction("LeftDPad_Left", IE_Released, this, &AVRPawn::LeftDPadPressed_Left);
	InputComponent->BindAction("LeftDPad_Right", IE_Released, this, &AVRPawn::LeftDPadPressed_Right);
	InputComponent->BindAction("DPadRight_Left", IE_Released, this, &AVRPawn::RightDPadPressed_Left);
	InputComponent->BindAction("DPadRight_Right", IE_Released, this, &AVRPawn::RightDPadPressed_Right);

	
}


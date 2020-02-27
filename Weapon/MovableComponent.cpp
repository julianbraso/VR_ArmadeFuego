// Fill out your copyright notice in the Description page of Project Settings.


#include "MovableComponent.h"

// Sets default values
UMovableComponent::UMovableComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
	/*root = CreateDefaultSubobject<USceneComponent>("Root");
	
	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	mesh->SetupAttachment(root);
	end = CreateDefaultSubobject<UStaticMeshComponent>("End");
	end->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	end->bHiddenInGame = true;
	end->SetupAttachment(root);*/
	
}

// Called when the game starts or when spawned
void UMovableComponent::BeginPlay()
{
	Super::BeginPlay();
	//root = this->GetOwner()->GetRootComponent();
	TArray<USceneComponent*> a;
	this->GetParentComponents(a);
	if (a.Num() > 0) {
		root = a[0];
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, a[0]->GetName(), true);
	}
	else {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "no parents...", true);
	}
	if (!bUsingY) {
		originalX = this->RelativeLocation.X;
	}
	else {
		originalX = this->RelativeLocation.Y;
	}
}

// Called every frame
void UMovableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType , ThisTickFunction);

	if (bHoldMovement) {
		return;
	}

	if (bisGrabbing) {
		if (grabbingHand != nullptr) {
			if (!bUsingY) {
				float v = (root->GetComponentTransform().InverseTransformPosition(grabbingHand->GetComponentLocation())).X - grabPos;
				if (v > fSlideBackLimitX && v <= originalX) {
					
					_a = false;
					
					this->SetRelativeLocation(FVector(v, this->RelativeLocation.Y, this->RelativeLocation.Z));

					//if (GEngine)
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::SanitizeFloat(v), true);
				}
				else {
					if (v <= fSlideBackLimitX) {
						if (!_a) {
							if (GEngine)
								GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "hit the limit!", true);
							EndLimitDelegate.Broadcast();
							_a = true;
						}
					}
				}
			}
			else {
				float v = (root->GetComponentTransform().InverseTransformPosition(grabbingHand->GetComponentLocation())).Y - grabPos;				//root->GetComponentTransform().InverseTransformPosition(grabbingHand->GetComponentLocation())).Y
				if (v > fSlideBackLimitX && v <= originalX) {
					_a = false;

					this->SetRelativeLocation(FVector(this->RelativeLocation.X, v ,this->RelativeLocation.Z));

					//if (GEngine)
						//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::SanitizeFloat(v), true);
				}
				else {
					if (v <= fSlideBackLimitX) {
						if (!_a) {
							if (GEngine)
								GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "hit the limit!", true);
							EndLimitDelegate.Broadcast();
							_a = true;
						}
					}
					/*if (GEngine)
						GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "hit the limit!", true);*/
				}
			}
		}
	}
	else {
		if (!bUsingY) {
			if (!FMath::IsNearlyEqual(this->RelativeLocation.X, originalX, 0.01f)) {			//  this->RelativeLocation.Y < originalX
				_b = false;
				this->SetRelativeLocation(FVector(0, FMath::FInterpTo(this->RelativeLocation.X, originalX, GetWorld()->DeltaTimeSeconds, fSlideBackSpeed), 0));

				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::SanitizeFloat(this->RelativeLocation.X), true);
			}

			else
			{
				if (!_b) {
					if (GEngine)
						GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "back to origin!", true);
					StartLimitDelegate.Broadcast();
					_b = true;
				}
			}
		}
		else {
			if (!FMath::IsNearlyEqual(this->RelativeLocation.Y, originalX, 0.01f)) {			//  this->RelativeLocation.Y < originalX
				_b = false;
				this->SetRelativeLocation(FVector(0, FMath::FInterpTo(this->RelativeLocation.Y,originalX,GetWorld()->DeltaTimeSeconds,fSlideBackSpeed), 0));

				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::SanitizeFloat(this->RelativeLocation.Y), true);
			}

			else
			 {
				if (!_b) {
					if (GEngine)
						GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "back to origin!", true);
					StartLimitDelegate.Broadcast();
					_b = true;
				}
			}
		}
	}
}

void UMovableComponent::Animate()
{
	if (!bUsingY) {
			//this->SetRelativeLocation(FVector(FMath::FInterpTo(this->RelativeLocation.X, originalX-fSlideBackLimitX, GetWorld()->DeltaTimeSeconds, fSlideBackSpeed*2), 0, 0));
		this->SetRelativeLocation(FVector(fSlideBackLimitX, this->RelativeLocation.Y, this->RelativeLocation.Z));
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::SanitizeFloat(this->RelativeLocation.X), true);
	}
	else {
		this->SetRelativeLocation(FVector(this->RelativeLocation.X, fSlideBackLimitX ,this->RelativeLocation.Z));
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString::SanitizeFloat(this->RelativeLocation.Y), true);
	}
	EndLimitDelegate.Broadcast();
	_b = false;
}

void UMovableComponent::GrabbedEvent_Implementation(USceneComponent* hand)
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "GrabEvent Called!", true);
	grabbingHand = hand;
	bisGrabbing = true;
	if (bUsingY) {
		grabPos = (root->GetComponentTransform().InverseTransformPosition(grabbingHand->GetComponentLocation())).Y;
	}
	else {
		grabPos = (root->GetComponentTransform().InverseTransformPosition(grabbingHand->GetComponentLocation())).X;
	}
	//grabPos = hand->GetComponentLocation();
	//mesh->AttachToComponent(grabbingHand,FAttachmentTransformRules::KeepRelativeTransform);

}

void UMovableComponent::ReleasedEvent_Implementation(USceneComponent* hand)
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "ReleasedEvent Called!", true);
	bisGrabbing = false;
	grabbingHand = nullptr;
	grabPos = originalX;
	//mesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
}


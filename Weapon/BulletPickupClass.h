// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TriggerPickup.h"
#include "Enums.h"
#include "BulletPickupClass.generated.h"

/**
 * 
 */
UCLASS()
class POLISIM_API ABulletPickupClass : public ATriggerPickup
{
	GENERATED_BODY()

public:
	ABulletPickupClass();
	UPROPERTY(EditAnywhere)
		EBulletCaliber bulletCaliber;

protected:
	
};

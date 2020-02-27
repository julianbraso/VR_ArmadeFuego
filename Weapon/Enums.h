// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Enums.generated.h"

UENUM(BlueprintType)
enum class EBulletCaliber : uint8 {
	Cal_9 UMETA(DisplayName = "9 mm"),
	Cal_45 UMETA(DisplayName = ".45")
};

/**
 * 
 */
UCLASS()
class POLISIM_API UEnums : public UObject
{
	GENERATED_BODY()
	
};

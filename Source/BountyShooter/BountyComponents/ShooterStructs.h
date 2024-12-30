// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterStructs.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct BOUNTYSHOOTER_API FGateSetting
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	float MaxMoveSpeed;
	UPROPERTY(BlueprintReadWrite)
	float MaxAcceleration;
	UPROPERTY(BlueprintReadWrite)
	float BrakingDeceleration;
	UPROPERTY(BlueprintReadWrite)
	float BrakingFrictionFactor;
	UPROPERTY(BlueprintReadWrite)
	float BrakingFriction;
	UPROPERTY(BlueprintReadWrite)
	bool UseSeperateBrakingFriction;
};

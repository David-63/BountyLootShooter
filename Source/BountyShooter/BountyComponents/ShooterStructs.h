// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimSequence.h"
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

USTRUCT(BlueprintType)
struct BOUNTYSHOOTER_API FDebugOptions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShowLocomotionData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShowGateData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DistanceMatching;
};

USTRUCT(BlueprintType)
struct BOUNTYSHOOTER_API FDirectionalAnimations
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequence* ForwardAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequence* BackwardAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequence* RightAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequence* LeftAnim;
};
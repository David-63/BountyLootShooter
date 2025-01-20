// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimSequence.h"
#include "Engine/DataTable.h"
#include "ShooterStructs.generated.h"

/**
 * 
 */

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RootYawOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShowFallingTime;
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

USTRUCT(BlueprintType)
struct BOUNTYSHOOTER_API FGateMovement : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxMoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAcceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakingDeceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakingFrictionFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakingFriction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool UseSeperateBrakingFriction;
};
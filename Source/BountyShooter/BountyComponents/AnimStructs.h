// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/BlendProfile.h"
#include "Animation/AnimationAsset.h"
#include "AnimStructs.generated.h"


/**
 *
 */
USTRUCT(BlueprintType)
struct BOUNTYSHOOTER_API FMovementDirectionThresholds
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	float FL;
	UPROPERTY(BlueprintReadWrite)
	float FR;
	UPROPERTY(BlueprintReadWrite)
	float BL;
	UPROPERTY(BlueprintReadWrite)
	float BR;
};

/**
 *
 */
USTRUCT(BlueprintType)
struct BOUNTYSHOOTER_API FBlendStackInputs
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAnimationAsset> Anim;
	UPROPERTY(BlueprintReadWrite)
	bool Loop;
	UPROPERTY(BlueprintReadWrite)
	float StartTime;
	UPROPERTY(BlueprintReadWrite)
	float BlendTime;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UBlendProfile> BlendProfile;
	UPROPERTY(BlueprintReadWrite)
	TArray<FName> Tags;
};

/**
 *
 */
USTRUCT(BlueprintType)
struct BOUNTYSHOOTER_API FCharacterInputState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool WantsToSprint;
	UPROPERTY(BlueprintReadWrite)
	bool WantsToWalk;
	UPROPERTY(BlueprintReadWrite)
	bool WantsToStrafe;
	UPROPERTY(BlueprintReadWrite)
	bool WantsToAim;
};
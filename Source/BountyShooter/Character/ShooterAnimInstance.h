// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MoveStateEnums.h"
#include "AnimStructs.h"
#include "PoseSearch/PoseSearchLibrary.h"
#include "PoseSearch/PoseSearchDatabase.h"
#include "ShooterAnimInstance.generated.h"

class AShooterCharacter;
class UCharacterTrajectoryComponent;
class UCharacterMovementComponent;



/**
 * 
 */
UCLASS()
class BOUNTYSHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite, category = "References")
	TObjectPtr<AShooterCharacter> ShooterCharacter = nullptr;
	UPROPERTY(BlueprintReadWrite, category = "References")
	TObjectPtr<UCharacterMovementComponent> ShooterMovement = nullptr;	

};

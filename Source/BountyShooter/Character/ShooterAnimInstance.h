// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimationInterface.h"
#include "BountyShooter/BountyComponents/ShooterEnums.h"

#include "ShooterAnimInstance.generated.h"

class AShooterCharacter;
class UCharacterMovementComponent;



/**
 * 
 */
UCLASS()
class BOUNTYSHOOTER_API UShooterAnimInstance : public UAnimInstance, public IShooterAnimationInterface
{
	GENERATED_BODY()
	
public:
	virtual void RecieveEquippedState(ELoadoutSlot Loadout) override;

protected:
	UPROPERTY(BlueprintReadWrite, category = "References")
	TObjectPtr<AShooterCharacter> ShooterCharacter = nullptr;
	UPROPERTY(BlueprintReadWrite, category = "References")
	TObjectPtr<UCharacterMovementComponent> ShooterMovement = nullptr;

public:
	UPROPERTY(BlueprintReadWrite, category = "Character enums")
	EEquippedState EquippedState = EEquippedState::EES_Unarmed;
};

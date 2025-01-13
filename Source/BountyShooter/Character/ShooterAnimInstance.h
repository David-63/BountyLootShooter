// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimationInterface.h"
#include "BountyShooter/BountyComponents/ShooterEnums.h"
#include "BountyShooter/BountyComponents/ShooterStructs.h"

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
	
	// 인터페이스 테스트용 (지금은 안쓰는 기능)
public:
	virtual void ReceiveEquippedState(EEquippedState EquipState) override;
	virtual void ReceiveCurrentGate(EGate Gate) override;
	virtual void ReceiveGroundDistance(float Distance) override;

protected:
	UPROPERTY(BlueprintReadWrite, category = "References")
	TObjectPtr<AShooterCharacter> ShooterCharacter = nullptr;
	UPROPERTY(BlueprintReadWrite, category = "References")
	TObjectPtr<UCharacterMovementComponent> ShooterMovement = nullptr;

public:
	UPROPERTY(BlueprintReadWrite, category = "Character enums")
	EEquippedState EquippedState = EEquippedState::EES_Unarmed;

	UPROPERTY(BlueprintReadWrite, category = "Character enums")
	EGate InComingGate = EGate::EG_Walk;
	UPROPERTY(BlueprintReadWrite, category = "Character enums")
	EGate CurrentGate = EGate::EG_Walk;
	UPROPERTY(BlueprintReadWrite, category = "Character enums")
	float GroundDistance = 0.f;


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Character Structs")
	FDebugOptions DebugOptions;

	UPROPERTY(BlueprintReadWrite, category = "Character Structs")
	ELocomotionDirections LocomotionDirection;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
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
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AShooterCharacter> ShooterCharacter = nullptr;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> ShooterMovement = nullptr;	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCharacterTrajectoryComponent> CharacterTrajectory = nullptr;



	UPROPERTY(BlueprintReadOnly)
	FVector Velocity;
	UPROPERTY(BlueprintReadOnly)
	float Direction;
	UPROPERTY(BlueprintReadOnly)
	float MoveSpeed;
	UPROPERTY(BlueprintReadOnly)
	bool bShouldMove;
	UPROPERTY(BlueprintReadOnly)
	bool bIsFall;
	UPROPERTY(BlueprintReadOnly)
	bool bIsCrouched;
	UPROPERTY(BlueprintReadOnly)
	bool bIsWeaponEquipped;
	UPROPERTY(BlueprintReadOnly)
	bool bIsADS;

	UPROPERTY(BlueprintReadOnly)
	bool bAcceleration;
	UPROPERTY(BlueprintReadOnly)
	bool bStartedMoving;
	UPROPERTY(BlueprintReadOnly)
	float Acceleration;
	UPROPERTY(BlueprintReadOnly)
	float LastFrameAcceleration;
	
	UPROPERTY(BlueprintReadOnly)
	float DisplacementSpeed;
	UPROPERTY(BlueprintReadOnly)
	FVector WorldLocation;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float _deltaTime) override;

	void UpdateVariables(float _deltaTime);
	void UpdateTrajectory();
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "CharacterTrajectoryComponent.h"


void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	this->ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	if (ShooterCharacter)
	{
		this->ShooterMovement = this->ShooterCharacter->GetCharacterMovement();
		this->CharacterTrajectory = this->ShooterCharacter->GetCharacterTrajectory();
	}
	
}

void UShooterAnimInstance::NativeUpdateAnimation(float _deltaTime)
{
	Super::NativeUpdateAnimation(_deltaTime);
	if (this->ShooterCharacter)
	{
		UpdateTrajectory();
		UpdateVariables(_deltaTime);
		//this->bShouldMove = this->MoveSpeed > 3.f && !this->ShooterMovement->GetCurrentAcceleration().IsNearlyZero();
		
	}

}

void UShooterAnimInstance::UpdateVariables(float _deltaTime)
{	
	this->LastFrameAcceleration = this->Acceleration;
	this->Acceleration = this->ShooterMovement->GetCurrentAcceleration().Length();

	if (0.01 < this->Acceleration)
	{
		this->bAcceleration = true;
	}
	else
	{
		this->bAcceleration = false;
	}

	if (this->bAcceleration && 0.01 >= this->LastFrameAcceleration)
	{
		this->bStartedMoving = true;
	}
	else
	{
		this->bStartedMoving = false;
	}
	this->Velocity = this->ShooterCharacter->GetVelocity();
	this->MoveSpeed = this->Velocity.Size();

	this->bIsFall = this->ShooterMovement->IsFalling();		
	this->DisplacementSpeed = UKismetMathLibrary::SafeDivide((this->ShooterCharacter->GetActorLocation() - this->WorldLocation).Length(), _deltaTime);
	this->WorldLocation = this->ShooterCharacter->GetActorLocation();
}

void UShooterAnimInstance::UpdateTrajectory()
{
}

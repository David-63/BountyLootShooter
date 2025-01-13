// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UShooterAnimInstance::ReceiveEquippedState(EEquippedState EquipState)
{	
	this->EquippedState = EquipState;
}

void UShooterAnimInstance::ReceiveCurrentGate(EGate Gate)
{
	this->InComingGate = Gate;
}

void UShooterAnimInstance::ReceiveGroundDistance(float Distance)
{
	GroundDistance = Distance;
}

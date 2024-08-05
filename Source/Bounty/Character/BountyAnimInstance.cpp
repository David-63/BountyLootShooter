// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyAnimInstance.h"
#include "BountyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBountyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	BountyCharacter = Cast<ABountyCharacter>(TryGetPawnOwner());
}

void UBountyAnimInstance::NativeUpdateAnimation(float _deltaTime)
{
	Super::NativeUpdateAnimation(_deltaTime);
	if (nullptr == BountyCharacter)
	{
		BountyCharacter = Cast<ABountyCharacter>(TryGetPawnOwner());
	}
	if (nullptr == BountyCharacter) return;

	// for locomotion
	FVector velocity = BountyCharacter->GetVelocity();
	velocity.Z = 0.f;
	MoveSpeed = velocity.Size();
	bIsInAir = BountyCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = BountyCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

	bIsWeaponEquipped = BountyCharacter->IsWeaponEquipped();
	bIsCrouched = BountyCharacter->bIsCrouched;
	bIsADS = BountyCharacter->IsADS();
}

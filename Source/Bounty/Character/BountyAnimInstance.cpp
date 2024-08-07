// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyAnimInstance.h"
#include "BountyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	FRotator aimRotation = BountyCharacter->GetBaseAimRotation();
	FRotator movementRotation = UKismetMathLibrary::MakeRotFromX(velocity);
	velocity.Z = 0.f;

	MoveSpeed = velocity.Size();
	bIsInAir = BountyCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = BountyCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

	bIsWeaponEquipped = BountyCharacter->IsWeaponEquipped();
	bIsCrouched = BountyCharacter->bIsCrouched;
	bIsADS = BountyCharacter->IsADS();


	// for strafing
	FRotator deltaMovementDirection = UKismetMathLibrary::NormalizedDeltaRotator(movementRotation, aimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, deltaMovementDirection, _deltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;

	// for lean
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BountyCharacter->GetActorRotation();
	const FRotator deltaCharacterDirection = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float rotationValue = deltaCharacterDirection.Yaw / _deltaTime;
	const float interpRotation = FMath::FInterpTo(Lean, rotationValue, _deltaTime, 6.f);
	Lean = FMath::Clamp(interpRotation, -90.f, 90.f);
}

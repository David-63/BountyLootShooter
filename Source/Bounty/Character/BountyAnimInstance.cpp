// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyAnimInstance.h"
#include "BountyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Bounty/Weapon/BaseWeapon.h"

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

	// for aim offset
	AO_Yaw = BountyCharacter->GetAO_Yaw();
	AO_Pitch = BountyCharacter->GetAO_Pitch();

	// for fabrik
	EquippedWeapon = BountyCharacter->GetEquippedWeapon();
	if (bIsWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BountyCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LefthandSocket"), ERelativeTransformSpace::RTS_World);
		FVector outPosition;
		FRotator outRotation;
		BountyCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, outPosition, outRotation);
		LeftHandTransform.SetLocation(outPosition);
		LeftHandTransform.SetRotation(FQuat(outRotation));
		
		if (BountyCharacter->IsLocallyControlled())
		{
			bIsLocallyControlled = true;
			FTransform rightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("Hand_R"), ERelativeTransformSpace::RTS_World);
			RightHandRotation = UKismetMathLibrary::FindLookAtRotation(rightHandTransform.GetLocation(), rightHandTransform.GetLocation() + (rightHandTransform.GetLocation() - BountyCharacter->GetHitTarget()));
		}
		else
		{
			bIsLocallyControlled = false;
		}

		// µð¹ö±ë¿ë
		FTransform muzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
		FVector muzzleX(FRotationMatrix(muzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));

		DrawDebugLine(GetWorld(), muzzleTipTransform.GetLocation(), muzzleTipTransform.GetLocation() + muzzleX * 1000.f, FColor::Red);
		DrawDebugLine(GetWorld(), muzzleTipTransform.GetLocation(), BountyCharacter->GetHitTarget(), FColor::Orange);
		
	}


	// for turning movement
	TurningInPlace = BountyCharacter->GetTurningInPlace();
}

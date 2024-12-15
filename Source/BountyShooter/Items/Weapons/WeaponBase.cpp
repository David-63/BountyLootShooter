// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "BountyShooter/Character/ShooterCharacter.h"


void AWeaponBase::AttachWeapon(AShooterCharacter* Character)
{
	ShooterCharacter = Character;
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh(), AttachmentRules, FName(TEXT("Hand")));

	PickupDisable();
}

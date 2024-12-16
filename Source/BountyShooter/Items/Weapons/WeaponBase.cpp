// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "BountyShooter/Character/ShooterCharacter.h"

void AWeaponBase::BindOwner(AShooterCharacter* Character)
{
	ShooterCharacter = Character;
	PickupDisable();
}

void AWeaponBase::DrawWeapon(FName SocketName)
{
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(ShooterCharacter->GetMesh(), AttachmentRules, SocketName);
}


void AWeaponBase::HolsterWeapon(FName SocketName)
{
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(ShooterCharacter->GetMesh(), AttachmentRules, SocketName);
}

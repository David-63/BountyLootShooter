// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "BountyShooter/Character/ShooterCharacter.h"
#include "BountyShooter/Character/ShooterInventoryHandler.h"
#include "Engine/SkeletalMeshSocket.h"
#include "../ItemMeshComponent.h"
#include "WeaponAmmo.h"
#include "WeaponPlatform.h"

void AWeaponBase::Equip(AShooterCharacter* Character, FName Socket)
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon Equip"));
	ShooterCharacter = Character;
	SetOwner(Character);
	ChangeItemState(EItemState::EIS_Equipped);

	EInventorySlot slot = ShooterCharacter->InventoryHandler->FindEmptyWeaponSlot(WeaponCategory);
	if (EInventorySlot::EIS_MAX != slot)
	{
		ShooterCharacter->InventoryHandler->BindWeaponSlot(this, slot);
	}
	else
	{
		ShooterCharacter->InventoryHandler->ReplaceWeaponSlot(this, slot);
	}
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

void AWeaponBase::SetPlatform(AWeaponPlatform* Platform)
{
	WeaponPlatform = Platform;
}

void AWeaponBase::SetAmmo(AWeaponAmmo* Ammo)
{
	WeaponAmmo = Ammo;
}

UItemMeshComponent* AWeaponBase::GetItemMeshComponent()
{
	return ItemMeshComponent;
}

AWeaponPlatform* AWeaponBase::GetPlatform()
{
	return WeaponPlatform;
}

AWeaponAmmo* AWeaponBase::GetAmmo()
{
	return WeaponAmmo;
}

void AWeaponBase::FireRound(const FVector& _hitTarget)
{
	// √÷º“ ¡∂∞«
	APawn* ownerPawn = Cast<APawn>(GetOwner());
	if (nullptr == ownerPawn) { UE_LOG(LogTemp, Warning, TEXT("Owner pawn is null.")); return; }
	UWorld* world = GetWorld();
	if (nullptr == world) { UE_LOG(LogTemp, Warning, TEXT("World is null.")); return; }

	// ªÁ∞› ¿Ã∆—∆Æ ¿Áª˝ (≈∫æ‡ø°º≠ «ÿ¡‡æﬂ«‘)
	WeaponAmmo->PlayFireParticle();

	// ≈∏∞›
	AController* instigatorController = ownerPawn->GetController();
	const USkeletalMeshSocket* muzzleSocket = ItemMeshComponent->GetSocketByName(FName("Muzzle"));
	if (muzzleSocket)
	{
		WeaponPlatform->FireHitscan(_hitTarget, instigatorController, muzzleSocket);
	}
	else { UE_LOG(LogTemp, Warning, TEXT("Muzzle socket not found.")); }

	// √—æÀ º“∏µ 
	WeaponPlatform->CycleCartridge();
}

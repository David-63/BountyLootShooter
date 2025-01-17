// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "BountyShooter/Character/ShooterCharacter.h"
#include "BountyShooter/Character/ShooterInventoryHandler.h"
#include "Engine/SkeletalMeshSocket.h"
#include "../ItemMeshComponent.h"
#include "AmmunitionComponent.h"
#include "PlatformComponent.h"

void AWeaponBase::Equip(AShooterCharacter* Character, FName Socket)
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon Equip"));
	ShooterCharacter = Character;
	SetOwner(Character);
	ChangeItemState(EItemState::EIS_Equipped);

	ELoadoutSlot slot = ShooterCharacter->InventoryHandler->FindEmptyWeaponSlot(WeaponCategory);
	if (ELoadoutSlot::ELS_MAX != slot)
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

void AWeaponBase::SetPlatformComponent(UPlatformComponent* Platform)
{
	WeaponPlatform = Platform;
	WeaponPlatform->Initialize(this);
}

void AWeaponBase::SetAmmunitionComponent(UAmmunitionComponent* Ammo)
{
	WeaponAmmunition = Ammo;
	WeaponAmmunition->Initialize(this);
}

UItemMeshComponent* AWeaponBase::GetItemMeshComponent()
{
	return ItemMeshComponent;
}

UPlatformComponent* AWeaponBase::GetPlatformComponent()
{
	return WeaponPlatform;
}

UAmmunitionComponent* AWeaponBase::GetAmmunitionComponent()
{
	return WeaponAmmunition;
}

void AWeaponBase::FireRound(const FVector& _hitTarget)
{
	// 최소 조건
	APawn* ownerPawn = Cast<APawn>(GetOwner());
	if (nullptr == ownerPawn) { UE_LOG(LogTemp, Warning, TEXT("Owner pawn is null.")); return; }
	UWorld* world = GetWorld();
	if (nullptr == world) { UE_LOG(LogTemp, Warning, TEXT("World is null.")); return; }

	// 사격 이팩트 재생 (탄약에서 해줘야함)
	WeaponAmmunition->PlayFireParticle();

	// 타격
	AController* instigatorController = ownerPawn->GetController();
	const USkeletalMeshSocket* muzzleSocket = ItemMeshComponent->GetSocketByName(FName("Muzzle"));
	if (muzzleSocket)
	{
		WeaponPlatform->FireHitscan(world, _hitTarget, instigatorController, muzzleSocket);
	}
	else { UE_LOG(LogTemp, Warning, TEXT("Muzzle socket not found.")); }

	// 총알 소모됨
	WeaponPlatform->CycleCartridge(world);
}

void AWeaponBase::Reload()
{
	WeaponPlatform->AmmoInsertion();
	
	if (WeaponPlatform->IsChamberLoaded())
	{
		UWorld* world = GetWorld();
		WeaponPlatform->CycleCartridge(world);
	}
}

float AWeaponBase::GetFireRate() const
{
	// 추후에 추가될 예정?
	return WeaponPlatform->GetBaseFireRate();
}

bool AWeaponBase::IsChamberEnable() const
{	
	return WeaponPlatform->IsChamberLoaded();
}

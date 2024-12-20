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

	// Todo
	/*
	UpdateExtraAmmo();
	PlayEquipWeaponSound();
	WeaponAutoReload();	
	*/
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

void AWeaponBase::FireRound(const FVector& _hitTarget)
{
	// 최소 조건
	APawn* ownerPawn = Cast<APawn>(GetOwner());
	if (nullptr == ownerPawn) return;
	UWorld* world = GetWorld();
	if (nullptr == world) return;

	// 이펙트 있으면 재생
	//PlayFireEffect(*world);
	// 총알 소모됨
	ChamberingRound();
	

	AController* instigatorController = ownerPawn->GetController();

	if (MuzzleFlashSocket)
	{
		// 총구 위치 찾기
		FTransform socketTransform = MuzzleFlashSocket->GetSocketTransform(ItemMeshComponent);
		FVector beginLocation = socketTransform.GetLocation();
		TMap<AShooterCharacter*, uint32> hitMap;
		if (bUsingHitScan)
		{
			FireHitscan(beginLocation, _hitTarget, *world, instigatorController, hitMap);
		}
		else
		{
			FireProjectile(beginLocation, _hitTarget, *world);
		}
	}
}

void AWeaponBase::ChamberingRound()
{
	// 약실 있으면 탄피 날리기
	if (bChamber)
	{
		WeaponAmmo->EjectCasing();
	}



	/*
	

	if (0 != AmmoCur)
	{
		bChamber = true;
	}
	else
	{
		bChamber = false;
	}
	AmmoCur = FMath::Clamp(AmmoCur - 1, 0, AmmoMax);
	SetHUDCurrentAmmo();
	UE_LOG(LogTemp, Warning, TEXT("Ammo Cur : %d"), GetAmmo());
	if (IsChamberEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("chamber Empty"));
	}


	*/
}

FVector AWeaponBase::TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget)
{
	return FVector();
}

FVector AWeaponBase::WeaponTraceHit(const FVector& _traceStart, const FVector& _hitTarget, FHitResult& _inOutHit)
{
	return FVector();
}

void AWeaponBase::FireHitscan(FVector& beginLocation, const FVector& _hitTarget, const UWorld& world, AController* instigatorController, TMap<AShooterCharacter*, uint32>& hitMap)
{
}

void AWeaponBase::FireProjectile(FVector& beginLocation, const FVector& _hitTarget, UWorld& world)
{
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ItemBase.h"


#include "WeaponBase.generated.h"

class AWeaponPlatform;
class AWeaponAmmo;
class UWeaponMeshComponent;

/**
 * 
 */
UCLASS()
class BOUNTYSHOOTER_API AWeaponBase : public AItemBase
{
	GENERATED_BODY()


public:
	virtual void Equip(AShooterCharacter* Character, FName Socket) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon Function")
	void DrawWeapon(FName SocketName);
	UFUNCTION(BlueprintCallable, Category = "Weapon Function")
	void HolsterWeapon(FName SocketName);


	// ���� ī�װ�
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base Value", meta = (AllowPrivateAccess = "true"))
	EWeaponCategory WeaponCategory = EWeaponCategory::EWC_MAX;
public:
	FORCEINLINE EWeaponCategory GetWeaponCategory() { return WeaponCategory; }

	// ���� ������Ʈ
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Attachment", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AWeaponPlatform> WeaponPlatform = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Attachment", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AWeaponAmmo> WeaponAmmo = nullptr;


	// ��ݿ� �ʿ��� ����
private:
	const USkeletalMeshSocket* MuzzleFlashSocket = nullptr;
	bool bUsingHitScan = false;
	bool bChamber = true;
	// ��� �Լ�
public:
	void FireRound(const FVector& _hitTarget);
	void ChamberingRound();
private:
	FVector TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget);
	FVector WeaponTraceHit(const FVector& _traceStart, const FVector& _hitTarget, FHitResult& _inOutHit);
	void FireHitscan(FVector& beginLocation, const FVector& _hitTarget, const UWorld& world, AController* instigatorController, TMap<AShooterCharacter*, uint32>& hitMap);
	void FireProjectile(FVector& beginLocation, const FVector& _hitTarget, UWorld& world);
};

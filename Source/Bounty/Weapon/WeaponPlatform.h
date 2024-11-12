// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "WeaponPlatform.generated.h"


class USoundCue;
/**
 * 
 */
UCLASS()
class BOUNTY_API AWeaponPlatform : public ABaseWeapon
{
	GENERATED_BODY()
		
private:
	UPROPERTY(EditAnywhere, Category = "Platform Addon")
	class UAnimationAsset* PlatformFireAnimation;
	UPROPERTY(EditAnywhere, Category = "Platform Addon")
	class UParticleSystem* HitScanTrail;

public:
	UPROPERTY(EditAnywhere, Category = "Platform Addon")
	USoundCue* PlatformEquipSound;


	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	bool bUseAuto = true;

	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	float AdsFov = 45.f;
	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	float AdsInterpSpeed = 30.f;
	FORCEINLINE float GetAdsFov() const { return AdsFov; }
	FORCEINLINE float GetAdsInterpSpeed() const { return AdsInterpSpeed; }


protected:
	UPROPERTY(EditAnywhere, Category = "Platform property")
	float PlatformDamage = 10.f;

	UPROPERTY(EditAnywhere, Category = "Platform Addon")
	UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere, Category = "Platform Addon")
	USoundCue* FireSound;


private:
	UPROPERTY(EditAnywhere, Category = "Platform Scatter")	// 유효 거리
	float EffectiveRange = 500.f;
	UPROPERTY(EditAnywhere, Category = "Platform Scatter")	// 산탄 범위
	float ScatterRadius = 25.f;
	UPROPERTY(EditAnywhere, Category = "Platform Scatter")	// 정밀도 사용여부
	bool bUseScatter = true;
	UPROPERTY(EditAnywhere, Category = "Platform Scatter")
	uint32 NumberOfPellets = 2;
protected:
	FVector TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget);
	FVector WeaponTraceHit(const FVector& _traceStart, const FVector& _hitTarget, FHitResult& _inOutHit);


private:
	//UPROPERTY(EditAnywhere)
	//TSubclassOf<class AWeaponAmmo> AWeaponAmmoClass;

	UPROPERTY(EditAnywhere, Category = "Platform Ammo")
	class UWeaponAmmo* WeaponAmmo;


public:
	virtual void FireRound(const FVector& _hitTarget) override;
	void PlayWeaponEffect();
	void HitScanFire(FVector& beginLocation, const FVector& _hitTarget, const UWorld& world, AController* instigatorController, TMap<ABountyCharacter*, uint32>& hitMap);


private:
	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	bool bIsHitScan = true;

public:
	AWeaponPlatform();
};

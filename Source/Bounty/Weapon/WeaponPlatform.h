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
	// fire
	UPROPERTY(EditAnywhere, Category = "Platform Addon")
	class UAnimationAsset* PlatformFireAnimation = nullptr;	
	UPROPERTY(EditAnywhere, Category = "Platform Addon")
	UParticleSystem* MuzzleFlash = nullptr;
	UPROPERTY(EditAnywhere, Category = "Platform Addon")
	USoundCue* FireSound = nullptr;
	
	// impact
	UPROPERTY(EditAnywhere, Category = "Platform Addon")
	UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere, Category = "Platform Addon")
	class USoundCue* ImpactSound;

	// hitscan
	UPROPERTY(EditAnywhere, Category = "Platform Addon")
	class UParticleSystem* HitScanTrail = nullptr;
public:
	UPROPERTY(EditAnywhere, Category = "Platform Addon")
	USoundCue* PlatformEquipSound = nullptr;
	

private:
	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	float AdsFov = 45.f;
	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	float AdsInterpSpeed = 30.f;
public:
	FORCEINLINE virtual float GetAdsFov() const override { return AdsFov; }
	FORCEINLINE virtual float GetAdsInterpSpeed() const override { return AdsInterpSpeed; }

private:
	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	bool bUsingAutoFire = true;
	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	bool bUsingScatter = true;
	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	bool bUsingScope = false;
	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	bool bUsingHitScan = true;
	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	bool bUsingMagazine = true;
public:
	FORCEINLINE virtual bool IsUsingAutoFire() const override { return bUsingAutoFire; }
	FORCEINLINE virtual bool IsUsingScatter() const override { return bUsingScatter; }
	FORCEINLINE virtual bool IsUsingHitScan() const override { return bUsingHitScan; }
	FORCEINLINE virtual bool IsUsingScope() const override { return bUsingScope; }
	FORCEINLINE virtual bool IsUsingMagazine() const override { return bUsingMagazine; }

private:
	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	float PlatformDamage = 10.f;
	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	float FireRate = 0.06f;

public:
	FORCEINLINE float GetPlatformDamage() const { return PlatformDamage; }
	FORCEINLINE virtual float GetFireRate() const override { return FireRate; }
private:
	UPROPERTY(EditAnywhere, Category = "Platform Scatter")	// 유효 거리
	float EffectiveRange = 500.f;
	UPROPERTY(EditAnywhere, Category = "Platform Scatter")	// 산탄 범위
	float ScatterRadius = 25.f;
	UPROPERTY(EditAnywhere, Category = "Platform Scatter")
	uint32 NumberOfPellets = 2;



public:
	virtual void FireRound(const FVector& _hitTarget) override;
private:
	FVector TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget);
	FVector WeaponTraceHit(const FVector& _traceStart, const FVector& _hitTarget, FHitResult& _inOutHit);
	void PlayFireEffect(UWorld& _world);
	void PlayImpactEffect(FHitResult& _bulletHit, const UWorld& _world);
	void FireHitscan(FVector& beginLocation, const FVector& _hitTarget, const UWorld& world, AController* instigatorController, TMap<ABountyCharacter*, uint32>& hitMap);
	void FireProjectile(FVector& beginLocation, const FVector& _hitTarget, UWorld& world);


};

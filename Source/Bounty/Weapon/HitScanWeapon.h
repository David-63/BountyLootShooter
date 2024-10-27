// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "HitScanWeapon.generated.h"



class UParticleSystem;
/**
 * 
 */
UCLASS()
class BOUNTY_API AHitScanWeapon : public ABaseWeapon
{
	GENERATED_BODY()
public:
	virtual void Fire(const FVector& _hitTarget) override;

private:
	UPROPERTY(EditAnywhere, Category = "Weapon property")
	float Damage = 7.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Addon")
	UParticleSystem* ImpactParticle;
	UPROPERTY(EditAnywhere, Category = "Weapon Addon")
	UParticleSystem* BeamParticle;

	UPROPERTY(EditAnywhere, Category = "Weapon Addon")
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, Category = "Weapon Addon")
	USoundCue* FireSound;
	UPROPERTY(EditAnywhere, Category = "Weapon Addon")
	USoundCue* HitSound;

	/*
	* Trace end with scatter
	*/
protected:
	FVector TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget);
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float EffectiveRange = 1500.f;
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float ScatterRadius = 100.f;
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter = false;

};

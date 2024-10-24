// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "HitScanWeapon.generated.h"

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
	class UParticleSystem* ImpactParticle;
};

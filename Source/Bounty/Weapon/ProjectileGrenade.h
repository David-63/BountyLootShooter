// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileGrenade.generated.h"

/**
 * 
 */
UCLASS()
class BOUNTY_API AProjectileGrenade : public AProjectile
{
	GENERATED_BODY()
	
public:
	AProjectileGrenade();

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnBounce(const FHitResult& _impactResult, const FVector& _impactVelocity);

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Addon")
	USoundCue* BounceSound;


public:
	virtual void Destroyed() override;

};

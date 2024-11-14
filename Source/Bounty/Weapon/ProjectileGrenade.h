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
	UPROPERTY(EditAnywhere, Category = "Projectile Addon")
	USoundCue* BounceSound;
	UPROPERTY(EditAnywhere, Category = "Projectile Properties")
	float QuickDestroyTime = 0.5f;
	bool FirstCollision = false;

public:
	virtual void Destroyed() override;

};

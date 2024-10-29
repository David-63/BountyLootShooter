// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class BOUNTY_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()
	
public:
	AProjectileRocket();
	virtual void Destroyed() override;
protected:
	virtual void BeginPlay() override;
	virtual void OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit);

	UPROPERTY(EditAnywhere, Category = "Weapon property")
	USoundCue* ProjectileLoopSound;
	UPROPERTY(EditAnywhere, Category = "Weapon property")
	USoundAttenuation* LoopSoundAttenuation;
	UPROPERTY()
	UAudioComponent* ProjectileLoopComponent;
	UPROPERTY(VisibleAnywhere, Category = "Weapon property")
	class UThrusterMovementComponent* ThrusterMovementComponent;

private:



};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BOUNTY_API AProjectile : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Weapon property")
	class UParticleSystem* Tracer;
	UPROPERTY()
	class UParticleSystemComponent* TracerComponent;


protected:
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;
	UPROPERTY(EditAnywhere, Category = "Weapon property")
	UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere, Category = "Weapon property")
	class USoundCue* ImpactSound;
	UPROPERTY(EditAnywhere, Category = "Weapon property")
	float Damage = 7.f;

protected:
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit);

public:
	virtual void Destroyed() override;


public:	
	AProjectile();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

};

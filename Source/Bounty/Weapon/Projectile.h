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

	UPROPERTY(EditAnywhere, Category = "Weapon property")
	float Damage = 7.f;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;
	UPROPERTY(EditAnywhere, Category = "Weapon Addon")
	UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere, Category = "Weapon Addon")
	class USoundCue* ImpactSound;
	UPROPERTY(EditAnywhere, Category = "Weapon Addon")
	class UNiagaraSystem* TrailSystem;
	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit);



	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;

	void SpawnTrailSystem();


	FTimerHandle DestroyTimer;
	UPROPERTY(EditAnywhere, Category = "Weapon property")
	float DestroyTime = 3.5f;
	void StartDestroyTimer();
	virtual void DestroyTimerFinished();

	UPROPERTY(EditAnywhere, Category = "Weapon property")
	float DamageInnerRadius = 300.f;
	UPROPERTY(EditAnywhere, Category = "Weapon property")
	float DamageOuterRadius = 700.f;
	
	void ExplodeDamage();



public:
	virtual void Destroyed() override;


public:	
	AProjectile();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

};

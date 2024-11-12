// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponRound.generated.h"

UCLASS()
class BOUNTY_API AWeaponRound : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;
protected:
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(VisibleAnywhere)
	class UThrusterMovementComponent* ThrusterMovementComponent;


	/*
	* visible
	*/
private:
	UPROPERTY(EditAnywhere, Category = "visible property")
	class UParticleSystem* BulletTracerSystem;
	UPROPERTY()
	class UParticleSystemComponent* BulletTracerComponent;


public:
	UPROPERTY(EditAnywhere, Category = "Round property")
	float TotalDamage;


private:
	UPROPERTY(EditAnywhere, Category = "Impact property")
	UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere, Category = "Impact property")
	class USoundCue* ImpactSound;


	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit);



	// if explosive
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RoundMesh;

	UPROPERTY(EditAnywhere, Category = "Explosive property")
	bool bUseExplosive = false;

	FTimerHandle DestroyTimer;
	UPROPERTY(EditAnywhere, Category = "Explosive property")
	float DestroyTime = 2.f;
	void StartDestroyTimer();
	void DestroyTimerFinished();

	UPROPERTY(EditAnywhere, Category = "Explosive property")
	float DamageInnerRadius = 300.f;
	UPROPERTY(EditAnywhere, Category = "Explosive property")
	float DamageOuterRadius = 700.f;

	void ExplodeDamage();

	// if grenade
	void OnBounce(const FHitResult& _impactResult, const FVector& _impactVelocity);

private:
	UPROPERTY(EditAnywhere, Category = "Explosive property")
	bool bUseBounce = false;
	UPROPERTY(EditAnywhere, Category = "Explosive Addon")
	USoundCue* BounceSound;

	// if rocket
protected:
	UPROPERTY(EditAnywhere, Category = "Explosive Addon")
	class UNiagaraSystem* ThrusterTrailSystem;
	UPROPERTY()
	class UNiagaraComponent* ThrusterTrailComponent;
	void SpawnTrailSystem();



	UPROPERTY(EditAnywhere, Category = "Explosive Addon")
	USoundCue* ProjectileLoopSound;
	UPROPERTY(EditAnywhere, Category = "Explosive Addon")
	USoundAttenuation* LoopSoundAttenuation;
	UPROPERTY()
	UAudioComponent* ProjectileLoopComponent;

public:	
	// Sets default values for this actor's properties
	AWeaponRound();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	virtual void Destroyed() override;

};

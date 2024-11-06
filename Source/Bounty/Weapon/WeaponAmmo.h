// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponAmmo.generated.h"


class USoundCue;

/*
* 총알 데미지(무기 데미지와 합산)
* 팰릿 개수							 이건 platform이 가져감
* 작동 방식(투사체, 히트스캔)
* 발사음
* 피격음
* Casing 클래스
*/












UCLASS()
class BOUNTY_API AWeaponAmmo : public AActor
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(EditAnywhere, Category = "Ammo Addon")
	class UBoxComponent* CollisionBox;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, Category = "Ammo Addon")
	class UParticleSystem* Tracer;
	UPROPERTY()
	class UParticleSystemComponent* TracerComponent;

protected:
	UPROPERTY(EditAnywhere, Category = "Ammo Addon")
	UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere, Category = "Ammo Addon")
	class USoundCue* ImpactSound;
	UPROPERTY(EditAnywhere, Category = "Ammo Addon")
	class UNiagaraSystem* TrailSystem;
	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent;
	void SpawnTrailSystem();




	UPROPERTY(EditAnywhere, Category = "Ammo property")
	float AmmoDamage = 5.f;	
	// 아마 온힛에 공격판정 함수나 조건문이 필요할듯
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit);


protected:
	// 폭발
	FTimerHandle DestroyTimer;
	UPROPERTY(EditAnywhere, Category = "Ammo property")
	float DestroyTime = 2.f;
	void StartDestroyTimer();
	void DestroyTimerFinished();

	UPROPERTY(EditAnywhere, Category = "Ammo property")
	float DamageInnerRadius = 300.f;
	UPROPERTY(EditAnywhere, Category = "Ammo property")
	float DamageOuterRadius = 700.f;
	void ExplodeDamage();


private:
	// 유탄 바운스
	UPROPERTY(EditAnywhere, Category = "Weapon Addon")
	USoundCue* BounceSound;
protected:
	UFUNCTION()
	void OnBounce(const FHitResult& _impactResult, const FVector& _impactVelocity);


protected:
	// 로켓 추진
	UPROPERTY(EditAnywhere, Category = "Weapon property")
	USoundCue* ProjectileLoopSound;
	UPROPERTY(EditAnywhere, Category = "Weapon property")
	USoundAttenuation* LoopSoundAttenuation;
	UPROPERTY()
	UAudioComponent* ProjectileLoopComponent;
	UPROPERTY(VisibleAnywhere, Category = "Weapon property")
	class UThrusterMovementComponent* ThrusterMovementComponent;


protected:
	UPROPERTY(EditAnywhere, Category = "Ammo Addon")
	UParticleSystem* ImpactParticle;
	UPROPERTY(EditAnywhere, Category = "Ammo Addon")
	UParticleSystem* BeamParticle;
	UPROPERTY(EditAnywhere, Category = "Ammo Addon")
	USoundCue* HitSound;





public:
	virtual void Destroyed() override;


public:	
	// Sets default values for this actor's properties
	AWeaponAmmo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

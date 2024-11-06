// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponAmmo.generated.h"


class USoundCue;

/*
* �Ѿ� ������(���� �������� �ջ�)
* �Ӹ� ����							 �̰� platform�� ������
* �۵� ���(����ü, ��Ʈ��ĵ)
* �߻���
* �ǰ���
* Casing Ŭ����
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
	// �Ƹ� ������ �������� �Լ��� ���ǹ��� �ʿ��ҵ�
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit);


protected:
	// ����
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
	// ��ź �ٿ
	UPROPERTY(EditAnywhere, Category = "Weapon Addon")
	USoundCue* BounceSound;
protected:
	UFUNCTION()
	void OnBounce(const FHitResult& _impactResult, const FVector& _impactVelocity);


protected:
	// ���� ����
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

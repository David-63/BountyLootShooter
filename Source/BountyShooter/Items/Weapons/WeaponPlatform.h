// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponPlatform.generated.h"

class AWeaponBase;

#define TRACE_LENGTH 10000

UCLASS()
class BOUNTYSHOOTER_API AWeaponPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void Initialize(AWeaponBase* Base);

private:
	TObjectPtr<AWeaponBase> WeaponBase = nullptr;



private:
	UPROPERTY(EditAnywhere, Category = "Platform Property")	// 유효 거리
	float EffectiveRange = 500.f;
	UPROPERTY(EditAnywhere, Category = "Platform Property")	// 산탄(집탄율) 범위
	float SpreadRadius = 25.f;
	UPROPERTY(EditAnywhere, Category = "Platform Property") // 멀티샷 개수
	uint32 PelletCount = 2;

	UPROPERTY(EditAnywhere, Category = "Platform Property")
	float PlatformBaseDamage = 5.f;


	UPROPERTY(EditAnywhere, Category = "Platform Property")
	uint32 ClipCapacity = 25.f;
	UPROPERTY(EditAnywhere, Category = "Platform Property")
	uint32 ClipSize = 25.f;
	
	bool bChamberLoaded = true;
public:
	void FireHitscan(const FVector& _hitTarget, AController* InstigatorController, const USkeletalMeshSocket* MuzzleSocket);
	FVector WeaponTraceHit(const FVector& _traceStart, const FVector& _hitTarget, FHitResult& _inOutHit);
	FVector TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget);
	void CycleCartridge();

	float GetTotalDamage();

};

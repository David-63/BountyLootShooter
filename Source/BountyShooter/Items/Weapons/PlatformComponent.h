// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"
#include "PlatformComponent.generated.h"


class AWeaponBase;

UCLASS( Blueprintable, BlueprintType, ClassGroup = (Weapon), meta=(BlueprintSpawnableComponent) )
class BOUNTYSHOOTER_API UPlatformComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlatformComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Initialize(AWeaponBase* Base);

private:
	TObjectPtr<AWeaponBase> WeaponBase = nullptr;

	UPROPERTY(EditAnywhere, Category = "Character Addon")
	UAnimMontage* FireMontage;
	UAnimMontage* ReloadMontage;

public:
	FORCEINLINE UAnimMontage* GetFireMontage() { return FireMontage; }
	FORCEINLINE UAnimMontage* GetReloadMontage() { return ReloadMontage; }
	

private:
	UPROPERTY(EditAnywhere, Category = "Platform Property")	// 유효 거리
		float EffectiveRange = 500.f;
	UPROPERTY(EditAnywhere, Category = "Platform Property")	// 산탄(집탄율) 범위
		float SpreadRadius = 25.f;
	UPROPERTY(EditAnywhere, Category = "Platform Property") // 멀티샷 개수
		uint32 PelletCount = 2;

	UPROPERTY(EditAnywhere, Category = "Platform Property")
	uint32 PlatformBaseClipCapacity = 25.f;	
	UPROPERTY(EditAnywhere, Category = "Platform Property")
	uint32 CurrentClipSize = 25.f;

	bool bChamberLoaded = true;

	UPROPERTY(EditAnywhere, Category = "Platform Property")
	float PlatformBaseDamage = 5.f;
	UPROPERTY(EditAnywhere, Category = "Platform Property")
	float PlatformBaseFireRate = 6.f;
public:
	FORCEINLINE float GetBaseFireRate() const { return PlatformBaseFireRate; }
	FORCEINLINE bool IsChamberLoaded() const { return bChamberLoaded; }

public:
	// Hit Ray 로직 1, 2, 3
	void FireHitscan(UWorld* World, const FVector& _hitTarget, AController* InstigatorController, const USkeletalMeshSocket* MuzzleSocket);
	FVector WeaponTraceHit(const FVector& _traceStart, const FVector& _hitTarget, FHitResult& _inOutHit);
	FVector TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget);
	
	// 매 사격마다 약실 갱신 및 탄피 배출 (Ammunition 에서 진행)
	void CycleCartridge(UWorld* World);


	// 여기서 계산하지 말고, Combat으로 전달할 예정
	float GetTotalDamage();
		
};

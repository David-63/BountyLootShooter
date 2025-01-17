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

	UPROPERTY(EditAnywhere, Category = "Montage Shooter")
	UAnimMontage* ShooterFireMontage;
	UPROPERTY(EditAnywhere, Category = "Montage Shooter")
	UAnimMontage* ShooterReloadMontage;

	UPROPERTY(EditAnywhere, Category = "Montage Weapon")
	UAnimationAsset* WeaponFireMontage;
	UPROPERTY(EditAnywhere, Category = "Montage Weapon")
	UAnimationAsset* WeaponReloadMontage;

public:
	void PlayFireMontage();
	void PlayReloadMontage();

	FORCEINLINE UAnimMontage* GetFireMontage() { return ShooterFireMontage; }
	FORCEINLINE UAnimMontage* GetReloadMontage() { return ShooterReloadMontage; }
	

private:
	UPROPERTY(EditAnywhere, Category = "Platform Property")	// ��ȿ �Ÿ�
	float EffectiveRange = 500.f;
	UPROPERTY(EditAnywhere, Category = "Platform Property")	// ��ź(��ź��) ����
	float SpreadRadius = 25.f;
	UPROPERTY(EditAnywhere, Category = "Platform Property") // ��Ƽ�� ����
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
	// Hit Ray ���� 1, 2, 3
	void FireHitscan(UWorld* World, const FVector& _hitTarget, AController* InstigatorController, const USkeletalMeshSocket* MuzzleSocket);
	FVector WeaponTraceHit(const FVector& _traceStart, const FVector& _hitTarget, FHitResult& _inOutHit);
	FVector TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget);
	
	// �� ��ݸ��� ��� ���� �� ź�� ���� (Ammunition ���� ����)
	void CycleCartridge(UWorld* World);

	void AmmoEjectCase(UWorld* World);


	// ���⼭ ������� ����, Combat���� ������ ����
	float GetTotalDamage();

public:
	void ChamberingRound();
	void AmmoInsertion();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "WeaponPlatform.generated.h"


class USoundCue;
/**
 * 
 */
UCLASS()
class BOUNTY_API AWeaponPlatform : public ABaseWeapon
{
	GENERATED_BODY()
	
	/*
	* ���� ���� ���(��: �ڵ�, ���ڵ�, �ܹ�)
		�ݵ� ����(Scatter)						add
		���� �ӵ�								add
		���� �ӵ�								not
		ũ�ν���� �̹���							add
		FOV(�þ߰�)								add
		WeaponAmmoClass(���� ź�� Ŭ����)			add
		���� ������								add
	*/
	
private:
	UPROPERTY(EditAnywhere, Category = "Platform Addon")
	class UAnimationAsset* PlatformFireAnimation;

public:
	UPROPERTY(EditAnywhere, Category = "Platform Addon")
	USoundCue* PlatformEquipSound;

	UPROPERTY(EditAnywhere, Category = "Platform Crosshairs")
	UTexture2D* CrosshairCenter;
	UPROPERTY(EditAnywhere, Category = "Platform Crosshairs")
	UTexture2D* CrosshairLeft;
	UPROPERTY(EditAnywhere, Category = "Platform Crosshairs")
	UTexture2D* CrosshairRight;
	UPROPERTY(EditAnywhere, Category = "Platform Crosshairs")
	UTexture2D* CrosshairTop;
	UPROPERTY(EditAnywhere, Category = "Platform Crosshairs")
	UTexture2D* CrosshairBottom;

	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	float FireRate = 0.12f;
	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	bool bUseAuto = true;
	virtual void FireRound(const FVector& _hitTarget);

	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	float AdsFov = 45.f;
	UPROPERTY(EditAnywhere, Category = "Platform Properties")
	float AdsInterpSpeed = 30.f;
	FORCEINLINE float GetAdsFov() const { return AdsFov; }
	FORCEINLINE float GetAdsInterpSpeed() const { return AdsInterpSpeed; }


protected:
	UPROPERTY(EditAnywhere, Category = "Platform property")
	float PlatformDamage = 10.f;

	UPROPERTY(EditAnywhere, Category = "Platform Addon")
	UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere, Category = "Platform Addon")
	USoundCue* FireSound;


private:
	UPROPERTY(EditAnywhere, Category = "Platform Scatter")	// ��ȿ �Ÿ�
	float EffectiveRange = 1500.f;
	UPROPERTY(EditAnywhere, Category = "Platform Scatter")	// ��ź ����
	float ScatterRadius = 25.f;
	UPROPERTY(EditAnywhere, Category = "Platform Scatter")	// ���е� ��뿩��
	bool bUseScatter = true;
	UPROPERTY(EditAnywhere, Category = "Platform Scatter")
	uint32 NumberOfPellets = 23;
protected:
	FVector TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget);
	void WeaponTraceHit(const FVector& _traceStart, const FVector& _hitTarget, FHitResult& _inOutHit);


private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeaponAmmo> AWeaponAmmoClass;
	

	// ���� �Ʒ��� WeaponAmmo

	
};

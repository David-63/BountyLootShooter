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
	* 무기 조작 방식(예: 자동, 반자동, 단발)
		반동 제어(Scatter)						add
		연사 속도								add
		장전 속도								not
		크로스헤어 이미지							add
		FOV(시야각)								add
		WeaponAmmoClass(무기 탄약 클래스)			add
		무기 데미지								add
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
	UPROPERTY(EditAnywhere, Category = "Platform Scatter")	// 유효 거리
	float EffectiveRange = 1500.f;
	UPROPERTY(EditAnywhere, Category = "Platform Scatter")	// 산탄 범위
	float ScatterRadius = 25.f;
	UPROPERTY(EditAnywhere, Category = "Platform Scatter")	// 정밀도 사용여부
	bool bUseScatter = true;
	UPROPERTY(EditAnywhere, Category = "Platform Scatter")
	uint32 NumberOfPellets = 23;
protected:
	FVector TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget);
	void WeaponTraceHit(const FVector& _traceStart, const FVector& _hitTarget, FHitResult& _inOutHit);


private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeaponAmmo> AWeaponAmmoClass;
	

	// 여기 아래는 WeaponAmmo

	
};

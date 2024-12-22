// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ItemBase.h"


#include "WeaponBase.generated.h"

class AWeaponPlatform;
class AWeaponAmmo;
class UWeaponMeshComponent;

/**
 * 
 */
UCLASS()
class BOUNTYSHOOTER_API AWeaponBase : public AItemBase
{
	GENERATED_BODY()


public:
	virtual void Equip(AShooterCharacter* Character, FName Socket) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon Function")
	void DrawWeapon(FName SocketName);
	UFUNCTION(BlueprintCallable, Category = "Weapon Function")
	void HolsterWeapon(FName SocketName);


	// 무기 카테고리
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base Value", meta = (AllowPrivateAccess = "true"))
	EWeaponCategory WeaponCategory = EWeaponCategory::EWC_MAX;
public:
	FORCEINLINE EWeaponCategory GetWeaponCategory() { return WeaponCategory; }

	// 하위 컴포넌트
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Attachment", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AWeaponPlatform> WeaponPlatform = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Attachment", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AWeaponAmmo> WeaponAmmo = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Item Initialize")
	void SetPlatform(AWeaponPlatform* Platform);
	UFUNCTION(BlueprintCallable, Category = "Item Initialize")
	void SetAmmo(AWeaponAmmo* Ammo);

	UItemMeshComponent* GetItemMeshComponent();
	AWeaponPlatform* GetPlatform();
	AWeaponAmmo* GetAmmo();

	// 사격에 필요한 변수
private:
	
	
	// 사격 함수
public:
	void FireRound(const FVector& _hitTarget);

};

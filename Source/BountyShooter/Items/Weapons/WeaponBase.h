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


	// ���� ī�װ�
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base Value", meta = (AllowPrivateAccess = "true"))
	EWeaponCategory WeaponCategory = EWeaponCategory::EWC_MAX;
public:
	FORCEINLINE EWeaponCategory GetWeaponCategory() { return WeaponCategory; }

	// ���� ������Ʈ
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

	// ��ݿ� �ʿ��� ����
private:
	
	
	// ��� �Լ�
public:
	void FireRound(const FVector& _hitTarget);

};

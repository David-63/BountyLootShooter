// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ItemBase.h"

#include "WeaponBase.generated.h"

class UPlatformComponent;
class UAmmunitionComponent;
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
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Attachment")
	TObjectPtr<UPlatformComponent> WeaponPlatform = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Attachment")
	TObjectPtr<UAmmunitionComponent> WeaponAmmunition = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Item Initialize")
	void SetPlatformComponent(UPlatformComponent* Platform);
	UFUNCTION(BlueprintCallable, Category = "Item Initialize")
	void SetAmmunitionComponent(UAmmunitionComponent* Ammo);

	UItemMeshComponent* GetItemMeshComponent();
	UPlatformComponent* GetPlatformComponent();
	UAmmunitionComponent* GetAmmunitionComponent();
	
public:
	void FireRound(const FVector& _hitTarget);
	void Reload();
	
public:
	float GetFireRate() const;
	bool IsChamberEnable() const;

};

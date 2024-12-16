// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ItemBase.h"
#include "BountyShooter/Character/ShooterEnums.h"
#include "WeaponBase.generated.h"

/**
 * 
 */
UCLASS()
class BOUNTYSHOOTER_API AWeaponBase : public AItemBase
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, Category = "Switching")
	void BindOwner(AShooterCharacter* Character);

	// �Ʒ� �� �Լ��� �޽��� ������� �ű� ����
	UFUNCTION(BlueprintCallable, Category = "Switching")
	void DrawWeapon(FName SocketName);
	UFUNCTION(BlueprintCallable, Category = "Switching")
	void HolsterWeapon(FName SocketName);


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Base Value", meta = (AllowPrivateAccess = "true"))
	EWeaponCategory WeaponCategory = EWeaponCategory::EWC_MAX;

public:
	FORCEINLINE EWeaponCategory GetWeaponCategory() { return WeaponCategory; }

};

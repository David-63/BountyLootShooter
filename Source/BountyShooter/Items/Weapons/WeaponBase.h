// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ItemBase.h"
#include "WeaponBase.generated.h"

/**
 * 
 */
UCLASS()
class BOUNTYSHOOTER_API AWeaponBase : public AItemBase
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AttachWeapon(AShooterCharacter* Character);
};

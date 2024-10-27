// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "ScatterWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BOUNTY_API AScatterWeapon : public AHitScanWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector& _hitTarget) override;

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	uint32 NumberOfPellets = 23;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponAmmo.generated.h"

class AWeaponRound;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOUNTY_API UWeaponAmmo : public UActorComponent
{
	GENERATED_BODY()



private:
	UPROPERTY(EditAnywhere, Category = "Ammo Addon")
	TSubclassOf<AWeaponRound> RoundClass;

	UPROPERTY(EditAnywhere, Category = "Ammo property")
	float AmmoDamage = 5.f;




public:
	TSubclassOf<AWeaponRound> GetRoundClass() { return RoundClass; }


public:	
	UWeaponAmmo();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};

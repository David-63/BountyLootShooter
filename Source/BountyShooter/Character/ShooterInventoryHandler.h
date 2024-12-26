// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BountyShooter/BountyComponents/ShooterEnums.h"
#include "ShooterInventoryHandler.generated.h"

struct FInputActionValue;
class UInputAction;
class AItemBase;
class AWeaponBase;
class AShooterCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup = (ShooterHandler), meta=(BlueprintSpawnableComponent) )
class BOUNTYSHOOTER_API UShooterInventoryHandler : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShooterInventoryHandler();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void BindInventoryHandler(AShooterCharacter* TargetCharacter);

	void EnableInventoryAction();
	void DisableInventoryAction();
private:
	UPROPERTY()
	TObjectPtr<AShooterCharacter> ShooterCharacter;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Action")
	class UInputMappingContext* IMC_InventoryHandler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Action")
	TObjectPtr<UInputAction> WeaponSwapAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Action")
	TObjectPtr<UInputAction> InterAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Action")
	TObjectPtr<UInputAction> DropAction;

private:
	void WeaponSwap(const FInputActionValue& Value);
	void SelectWeaponDrop();

	void SwapWeapon(ELoadoutSlot NextWeaponSlot);
	bool isWeaponArmed = false;
	
	void ToggleWeapon();
	ELoadoutSlot GetOccupiedWeaponSlot();
	

public:	
	void PickupItem(AItemBase* Item);

	ELoadoutSlot FindEmptyWeaponSlot(EWeaponCategory WeaponCategory);
	void BindWeaponSlot(AWeaponBase* Item, ELoadoutSlot EquippableSlot);
	void ReplaceWeaponSlot(AWeaponBase* Item, ELoadoutSlot EquippableSlot);
	

protected:
	bool DrawSelectWeapon();
	bool HolsterSelectWeapon();

public:
	UPROPERTY()
	TMap<ELoadoutSlot, TObjectPtr<AWeaponBase>> WeaponSlots;
	
	UPROPERTY()
	ELoadoutSlot SelectedWeaponSlot = ELoadoutSlot::ELS_MAX;

	UPROPERTY()
	TMap<EWeaponSpecificType, int32> ExtraAmmoMap;
	UPROPERTY(EditAnywhere, Category = "Extra Ammo")
	int32 StartingAmmoAR = 6;
	UPROPERTY(EditAnywhere, Category = "Extra Ammo")
	int32 StartingAmmoPistol = 5;
	UPROPERTY(EditAnywhere, Category = "Extra Ammo")
	int32 StartingAmmoSMG = 6;
	UPROPERTY(EditAnywhere, Category = "Extra Ammo")
	int32 StartingAmmoScatterGun = 20;
public:
	AWeaponBase* GetSelectedWeapon();
	FName GetHolsterSocketName();

	// 나중에 인벤토리 UI 만들면 Context로 조작키를 덮어씌우게 할 예정

};

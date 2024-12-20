// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterEnums.h"
#include "ShooterInventoryHandler.generated.h"

class AItemBase;
class AWeaponBase;
class AShooterCharacter;
class UInputAction;

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
	TObjectPtr<UInputAction> PrimaryAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Action")
	TObjectPtr<UInputAction> SecondaryAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Action")
	TObjectPtr<UInputAction> SidarmAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Action")
	TObjectPtr<UInputAction> InterAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Action")
	TObjectPtr<UInputAction> DropAction;

private:
	void SelectPrimary();
	void SelectSecondary();
	void SelectSidarm();
	void SelectWeaponDrop();

	void SwapWeapon(EInventorySlot NextWeaponSlot);
	bool isWeaponArmed = false;
	
	void ToggleWeapon();
	EInventorySlot GetOccupiedWeaponSlot();
	

public:	
	void PickupItem(AItemBase* Item);

	EInventorySlot FindEmptyWeaponSlot(EWeaponCategory WeaponCategory);
	void BindWeaponSlot(AWeaponBase* Item, EInventorySlot EquippableSlot);
	void ReplaceWeaponSlot(AWeaponBase* Item, EInventorySlot EquippableSlot);
	

protected:
	bool DrawSelectWeapon();
	bool HolsterSelectWeapon();

	void ReplaceAndDestroyWeapon(EInventorySlot EquippableSlot, AWeaponBase* Weapon);
	void ReplaceWeapon(EInventorySlot EquippableSlot, AWeaponBase* Weapon);



public:
	UPROPERTY()
	TMap<EInventorySlot, TObjectPtr<AWeaponBase>> WeaponSlots;
	
	UPROPERTY()
	EInventorySlot SelectedWeaponSlot = EInventorySlot::EIS_Primary;

public:
	AWeaponBase* GetSelectedWeapon();
	FName GetHolsterSocketName();

	// 나중에 인벤토리 UI 만들면 Context로 조작키를 덮어씌우게 할 예정

};

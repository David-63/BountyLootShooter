// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BountyShooter/BountyComponents/InteractInterface.h"
#include "BountyShooter/BountyComponents/ShooterEnums.h"

#include "ItemBase.generated.h"

class AShooterCharacter;
class UPickupComponent;
class UItemMeshComponent;

UCLASS()
class BOUNTYSHOOTER_API AItemBase : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// interface
	virtual void Interact() override { return; }

	/*
	*	Actor Components
	*/
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPickupComponent> PickupArea = nullptr;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UItemMeshComponent> ItemMeshComponent = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Item Initialize")
	void SetPickupComponent(UPickupComponent* Pickup);
	UFUNCTION(BlueprintCallable, Category = "Item Initialize")
	void SetItemMeshComponent(UItemMeshComponent* Mesh);
	

	/*
	*	Actors
	*/
public:
	UPROPERTY()
	TObjectPtr<AShooterCharacter> ShooterCharacter = nullptr;

protected:
	UPROPERTY()
	EItemState ItemState = EItemState::EIS_Initial;
	UPROPERTY()
	EItemType ItemType = EItemType::EIT_Weapon;

public:
	FORCEINLINE EItemType GetItemType() { return ItemType; }

public:
	void ChangeItemState(EItemState State);
	virtual void Equip(AShooterCharacter* Character, FName Socket = FName());
	virtual void Drop();

};

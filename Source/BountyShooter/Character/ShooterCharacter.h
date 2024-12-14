// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MoveStateEnums.h"
#include "AnimStructs.h"
#include "ShooterCharacter.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UCharacterTrajectoryComponent;
class UWidgetComponent;

class UShooterMovementHandler;
class UShooterCombatHandler;
class UShooterInventoryHandler;

class AItemBase;

UCLASS()
class BOUNTYSHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShooterCharacter();
	virtual void Tick(float DeltaTime) override;


protected:
	virtual void BeginPlay() override;
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// components
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))	 
	TObjectPtr<UWidgetComponent> OverheadWidget = nullptr;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm3P = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera3P = nullptr;
public:
	FORCEINLINE TObjectPtr<USpringArmComponent> GetTpsSpringArm() const { return SpringArm3P; }
	FORCEINLINE TObjectPtr<UCameraComponent> GetTpsCamera() const { return Camera3P; }




private:
	FVector HitLocation;
	TObjectPtr<AActor> HitTarget;
	void LineTraceViewDirection(FHitResult& result);
public:
	FORCEINLINE const FVector& GetHitLocation() { return HitLocation; }
	FORCEINLINE const TObjectPtr<AActor>& GetHitTarget() { return HitTarget; }	
	
	// actor components
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Handler, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UShooterMovementHandler> MovementHandler = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Handler, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UShooterCombatHandler> CombatHandler = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Handler, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UShooterInventoryHandler> InventoryHandler = nullptr;


	// inventory function
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TArray<AItemBase*> OverlappingItems;
	
public:
	void SetOverlappingItems(AItemBase* Item, bool bShouldAdd = true);




	/*
	*	inputs
	*/
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InterAction = nullptr;

protected:
	void Look(const FInputActionValue& Value);
	void Interaction();

	bool IsUsingGamepad() const;


};

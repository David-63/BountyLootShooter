// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

#define TRACE_LENGTH 10000.f

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponSwap, EEquippedState, EquipState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeGate, EGate, CurrentGate);

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UCharacterTrajectoryComponent;
class UWidgetComponent;

class UShooterAnimInstance;
class UShooterMovementHandler;
class UShooterCombatHandler;
class UShooterInventoryHandler;

class UInteractDotWidget;
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
	UFUNCTION(BlueprintCallable, Category = "Animation")
	UShooterAnimInstance* GetCharacterAnimInstance() const;

	// 델리게이트로 BP에 EquipState를 전달하여 애니메이션 레이어 변경하는 용도
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponSwap OnWeaponSwapDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnChangeGate OnChangeGateDelegate;

	// 이건 안쓸듯
private:
	EEquippedState EquippedState;
	EGate CurrentGate;



	// raycast 값을 Combat에 전달 (비무장 상태에서도 사용하기 위해 여기에서 구현함)
private:
	FVector HitLocation;
	TObjectPtr<AActor> HitTarget;
	void LineTraceViewDirection(FHitResult& result);

public:
	FORCEINLINE const FVector& GetHitLocation() { return HitLocation; }
	FORCEINLINE const TObjectPtr<AActor>& GetHitTarget() { return HitTarget; }	
	
	// BP에서 제어할 수 있도록 추가함 (자체적으로 Init함수를 가짐)
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Handler, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UShooterMovementHandler> MovementHandler = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Handler, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UShooterCombatHandler> CombatHandler = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Handler, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UShooterInventoryHandler> InventoryHandler = nullptr;


	// InteractInterface를 상속받은 객체가 Ray에 닿으면 UI를 화면에 출력됨
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShooterUI, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInteractDotWidget> InteractDotWidget = nullptr;
	


	// 플레이어가 습득 가능한 범위내의 아이템이 있으면 후보 배열에 추가됨
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


public:
	void EnableCombatAction();
	void DisableCombatAction();
};

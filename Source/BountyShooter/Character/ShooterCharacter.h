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

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))	 
	TObjectPtr<UWidgetComponent> OverheadWidget = nullptr;
	

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm3P = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera3P = nullptr;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<USpringArmComponent> SpringArm1P = nullptr;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UCameraComponent> Camera1P = nullptr;

	//FTransform TpsRelativeTransform;
	//FTransform FpsRelativeTransform;
public:
	FORCEINLINE TObjectPtr<USpringArmComponent> GetTpsSpringArm() const { return SpringArm3P; }
	FORCEINLINE TObjectPtr<UCameraComponent> GetTpsCamera() const { return Camera3P; }
	//FORCEINLINE TObjectPtr<USpringArmComponent> GetFpsSpringArm() const { return SpringArm1P; }
	//FORCEINLINE TObjectPtr<UCameraComponent> GetFpsCamera() const { return Camera1P; }
	//FORCEINLINE FTransform GetTpsRelativeTransform() const { return TpsRelativeTransform; }
	//FORCEINLINE FTransform GetFpsRelativeTransform() const { return FpsRelativeTransform; }


private:
	TObjectPtr<UShooterMovementHandler> MovementHandler;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InterAction = nullptr;

protected:	
	void Look(const FInputActionValue& Value);

	bool IsUsingGamepad() const;

};

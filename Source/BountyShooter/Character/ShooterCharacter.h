// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UCharacterTrajectoryComponent;
class UWidgetComponent;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Widget", meta = (AllowPrivateAccess = "true"))	 
	TObjectPtr<UWidgetComponent> OverheadWidget = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character MotionMatching", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterTrajectoryComponent> CharacterTrajectory = nullptr;

public:
	TObjectPtr<UCharacterTrajectoryComponent> GetCharacterTrajectory() const { return this->CharacterTrajectory; }


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm3P = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera3P = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm1P = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera1P = nullptr;

	FTransform TpsRelativeTransform;
	FTransform FpsRelativeTransform;
public:
	FORCEINLINE TObjectPtr<USpringArmComponent> GetTpsSpringArm() const { return SpringArm3P; }
	FORCEINLINE TObjectPtr<UCameraComponent> GetTpsCamera() const { return Camera3P; }
	FORCEINLINE TObjectPtr<USpringArmComponent> GetFpsSpringArm() const { return SpringArm1P; }
	FORCEINLINE TObjectPtr<UCameraComponent> GetFpsCamera() const { return Camera1P; }
	FORCEINLINE FTransform GetTpsRelativeTransform() const { return TpsRelativeTransform; }
	FORCEINLINE FTransform GetFpsRelativeTransform() const { return FpsRelativeTransform; }

private:
	float RunScale = 1.f;
	float WalkScale = 0.5f;
	float RunScaleThreshold = 0.75f;


	FVector2D ClampInputScale(FVector2D& _inputScale);


private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction>  MoveAction = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InterAction = nullptr;

protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	bool IsUsingGamepad() const;


	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//class UCombatComponent* Combat;
	
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//class UCombatComponent* Movement;

};

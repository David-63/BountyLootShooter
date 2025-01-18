// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BountyShooter/BountyComponents/ShooterStructs.h"
#include "BountyShooter/BountyComponents/ShooterEnums.h"

#include "ShooterMovementHandler.generated.h"

class AShooterCharacter;
class UInputAction;
struct FInputActionValue;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(ShooterHandler), meta=(BlueprintSpawnableComponent) )
class BOUNTYSHOOTER_API UShooterMovementHandler : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShooterMovementHandler();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CheckGround();

	UFUNCTION(BlueprintCallable)
	void BindMovementHandler(AShooterCharacter* TargetCharacter);

	void EnableMovementAction();
	void DisableMovementAction();


private:
	UPROPERTY()
	TObjectPtr<AShooterCharacter> ShooterCharacter;
		
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Input")
	class UInputMappingContext* IMC_MovementHandler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Input")
	TObjectPtr<UInputAction> StanceAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Input")
	TObjectPtr<UInputAction> GaitAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Input")
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Input")
	TObjectPtr<UInputAction> CameraSwapAction;


private:
	bool bIsCrouched = false;
	bool bIsAimed = false;

	float BaseMovespeed = 450.f;
	float SprintMovespeed = 800.f;
	float AdsMovespeed = 250.f;

	EGate CurrentGate;
	UDataTable* GateDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Movement", meta = (AllowPrivateAccess = "true"))
	float DefaultArmLength = 150;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Movement", meta = (AllowPrivateAccess = "true"))
	float AimArmLength = 75;


	float TransitionTimeCur = 0.f;
	bool DoTransition = false;

public:
	void UpdateGate(EGate Gate);
	void BeginTransition();

private:
	void TickTransition(float DeltaTime);
	
public:
	void Move(const FInputActionValue& Value);
	void Jump();
	void StopJumping();
	/* Jump */
	// Crouch, Dodge로 나눠질 예정
	//void Stance();
	void Crouch();
	void UnCrouch();
	void Dodge();
	// Walk, Sprint로 나눠질 예정
	//void Gait();		
	void Sprint();
	void Jog();
	void AimHold();
	void AimRelease();
	void CameraSwap();
};

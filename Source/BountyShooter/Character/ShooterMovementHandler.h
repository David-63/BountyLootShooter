// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterMovementHandler.generated.h"

class AShooterCharacter;
class UInputAction;
struct FInputActionValue;

UCLASS( ClassGroup=(ShooterHandler), meta=(BlueprintSpawnableComponent) )
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

	UFUNCTION(BlueprintCallable)
	void MappingMovementContext(AShooterCharacter* TargetCharacter);

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


public:
	void Move(const FInputActionValue& Value);
	void Jump();
	void StopJumping();
	/* Jump */
	void Stance();		// Crouch, Dodge�� ������ ����
	void Gait();		// Walk, Sprint�� ������ ����
	void Aim();
	void CameraSwap();
};
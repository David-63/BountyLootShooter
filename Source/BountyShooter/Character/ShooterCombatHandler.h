// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterCombatHandler.generated.h"

class AShooterCharacter;
class UInputAction;
struct FInputActionValue;

#define TRACE_LENGTH 10000.f


UCLASS(Blueprintable, BlueprintType, ClassGroup=(ShooterHandler), meta=(BlueprintSpawnableComponent) )
class BOUNTYSHOOTER_API UShooterCombatHandler : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShooterCombatHandler();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void BindCombatHandler(AShooterCharacter* TargetCharacter);

	void EnableCombatAction();
	void DisableCombatAction();

private:
	UPROPERTY()
	TObjectPtr<AShooterCharacter> ShooterCharacter;

	// 아마 안쓰게 될 기능
	bool bIsWeaponDrawn = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Input")
	class UInputMappingContext* IMC_CombatHandler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Input")
	TObjectPtr<UInputAction> MeleeAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Input")
	TObjectPtr<UInputAction> ThrowAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Input")
	TObjectPtr<UInputAction> ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Input")
	TObjectPtr<UInputAction> InterAction;
	
public:
	void ChamberingRound();
	void AmmoInsertion();
	void Fire();
	void Melee();
	void Throw();
	void WeaponDraw();
};

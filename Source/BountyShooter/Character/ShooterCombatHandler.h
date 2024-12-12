// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterCombatHandler.generated.h"

class AShooterCharacter;
class UInputAction;
struct FInputActionValue;

UCLASS( ClassGroup=(ShooterHandler), meta=(BlueprintSpawnableComponent) )
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
	void MappingCombatContext(AShooterCharacter* TargetCharacter);

private:
	UPROPERTY()
	TObjectPtr<AShooterCharacter> ShooterCharacter;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Input")
	class UInputMappingContext* IMC_MovementHandler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Input")
	TObjectPtr<UInputAction> MeleeAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Input")
	TObjectPtr<UInputAction> ThrowAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Input")
	TObjectPtr<UInputAction> ReloadAction;

public:
	void ChamberingRound();
	void AmmoInsertion();
	void Fire();
	void Melee();
	void Throw();
};

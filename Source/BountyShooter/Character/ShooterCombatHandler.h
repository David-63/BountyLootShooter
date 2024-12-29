// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterCombatHandler.generated.h"

class AShooterCharacter;
class UInputAction;
struct FInputActionValue;



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

	
public:
	void ChamberingRound();
	void AmmoInsertion();
	void Fire();
	void Melee();
	void Throw();


protected:
	FTimerHandle FireTimer = FTimerHandle();

	bool bFireCooldownFinished = true;
	bool bIsAttackDown;
	void FireTimerStart();
	void FireTimerFinished();
	bool CanFire();
};

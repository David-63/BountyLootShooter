// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterInventoryHandler.generated.h"

class AItemBase;
class AShooterCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup = (ShooterHandler), meta=(BlueprintSpawnableComponent) )
class BOUNTYSHOOTER_API UShooterInventoryHandler : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShooterInventoryHandler();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void MappingInventoryContext(AShooterCharacter* TargetCharacter);

private:
	UPROPERTY()
	TObjectPtr<AShooterCharacter> ShooterCharacter;


public:
	UPROPERTY()
	TObjectPtr<AItemBase> Primary;

};

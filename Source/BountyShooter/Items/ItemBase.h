// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

class AShooterCharacter;
class UPickupComponent;

UCLASS()
class BOUNTYSHOOTER_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	UPROPERTY()
	TObjectPtr<AShooterCharacter> ShooterCharacter;
	UPROPERTY()
	TObjectPtr<UPickupComponent> PickupArea;


public:
	void PickupDisable();
	void PickupEnable();
	
};

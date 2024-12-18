// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "BountyShooter/Character/ShooterCharacter.h"
#include "PickupComponent.generated.h"



/**
 * 
 */

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Items), meta = (BlueprintSpawnableComponent))
class BOUNTYSHOOTER_API UPickupComponent : public USphereComponent
{
	GENERATED_BODY()
public:
	UPickupComponent();

protected:
	/** Called when the game starts */
	virtual void BeginPlay() override;

	/** Code for when something overlaps this component */
	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:
	UFUNCTION()
	void PickupDisable();

	UFUNCTION()
	void PickupEnable();
	
};

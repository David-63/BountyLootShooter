// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponRound.generated.h"

UCLASS()
class BOUNTY_API AWeaponRound : public AActor
{
	GENERATED_BODY()
	

public:	
	// Sets default values for this actor's properties
	AWeaponRound();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	virtual void Destroyed() override;

};

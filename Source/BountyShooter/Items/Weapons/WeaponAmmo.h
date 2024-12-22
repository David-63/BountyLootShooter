// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponAmmo.generated.h"

class AWeaponBase;
class ACartridgeCase;

UCLASS()
class BOUNTYSHOOTER_API AWeaponAmmo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponAmmo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void Initialize(AWeaponBase* Base);

private:
	TObjectPtr<AWeaponBase> WeaponBase = nullptr;
	TSubclassOf<ACartridgeCase> CartridgeCase;

public:
	void EjectCase(const USkeletalMeshSocket* EjectSocket);
	void UpdateChamber(uint32& ClipSize, uint32 ClipCapacity, bool& bChamberLoaded);


public:
	void PlayFireParticle();
	void PlayTrailParticle();
	void PlayImpactParticle();



};

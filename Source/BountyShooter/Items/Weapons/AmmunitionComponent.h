// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AmmunitionComponent.generated.h"

class AWeaponBase;
class ACartridgeCase;

UCLASS( Blueprintable, BlueprintType, ClassGroup=(Weapon), meta=(BlueprintSpawnableComponent) )
class BOUNTYSHOOTER_API UAmmunitionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAmmunitionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Initialize(AWeaponBase* Base);

private:
	TObjectPtr<AWeaponBase> WeaponBase = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACartridgeCase> CartridgeCase = nullptr;

public:
	void EjectCase(const USkeletalMeshSocket* EjectSocket, UWorld* World);
	void UpdateChamber(uint32& ClipSize, uint32 ClipCapacity, bool& bChamberLoaded);


public:
	void PlayFireParticle();
	void PlayTrailParticle();
	void PlayImpactParticle();

};

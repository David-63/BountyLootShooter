// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAmmo.h"
#include "WeaponBase.h"
#include "../ItemMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "CartridgeCase.h"

// Sets default values
AWeaponAmmo::AWeaponAmmo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AWeaponAmmo::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponAmmo::Initialize(AWeaponBase* Base)
{
	WeaponBase = Base;
}

void AWeaponAmmo::EjectCase(const USkeletalMeshSocket* EjectSocket)
{
	FTransform socketTransform = EjectSocket->GetSocketTransform(WeaponBase->GetItemMeshComponent());

	UWorld* world = GetWorld();
	if (!world) return;

	FActorSpawnParameters spawnParam;
	spawnParam.Owner = this;
	ACartridgeCase* cartridgeCase = world->SpawnActor<ACartridgeCase>(CartridgeCase, socketTransform.GetLocation(), socketTransform.GetRotation().Rotator(), spawnParam);

	if (!cartridgeCase) return;
	cartridgeCase->CasingImpulse();
}

void AWeaponAmmo::UpdateChamber(uint32& ClipSize, uint32 ClipCapacity, bool& bChamberLoaded)
{
	if (ClipSize > 0)
	{
		bChamberLoaded = true;
	}
	else
	{
		bChamberLoaded = false;
	}
	
	ClipSize = FMath::Clamp(ClipSize - 1, 0, ClipCapacity);
}

void AWeaponAmmo::PlayFireParticle()
{
}

void AWeaponAmmo::PlayTrailParticle()
{
}

void AWeaponAmmo::PlayImpactParticle()
{
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmunitionComponent.h"

#include "WeaponBase.h"
#include "../ItemMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"

#include "CartridgeCase.h"

// Sets default values for this component's properties
UAmmunitionComponent::UAmmunitionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UAmmunitionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAmmunitionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAmmunitionComponent::Initialize(AWeaponBase* Base)
{
	WeaponBase = Base;
}

void UAmmunitionComponent::EjectCase(const USkeletalMeshSocket* EjectSocket, UWorld* World)
{
	FTransform socketTransform = EjectSocket->GetSocketTransform(WeaponBase->GetItemMeshComponent());

	FActorSpawnParameters spawnParam;
	spawnParam.Owner = WeaponBase;

	if (CartridgeCase)
	{
		ACartridgeCase* cartridgeCase = World->SpawnActor<ACartridgeCase>(CartridgeCase, socketTransform.GetLocation(), socketTransform.GetRotation().Rotator(), spawnParam);
		if (!cartridgeCase) return;
		cartridgeCase->CasingImpulse();
	}
}

void UAmmunitionComponent::UpdateChamber(uint32& ClipSize, uint32 ClipCapacity, bool& bChamberLoaded)
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

void UAmmunitionComponent::PlayFireParticle()
{
}

void UAmmunitionComponent::PlayTrailParticle()
{
}

void UAmmunitionComponent::PlayImpactParticle()
{
}


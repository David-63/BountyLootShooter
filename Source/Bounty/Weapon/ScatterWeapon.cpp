// Fill out your copyright notice in the Description page of Project Settings.


#include "ScatterWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Bounty/Character/BountyCharacter.h"

void AScatterWeapon::Fire(const FVector& _hitTarget)
{
	ABaseWeapon::Fire(_hitTarget);

	APawn* ownerPawn = Cast<APawn>(GetOwner());
	if (nullptr == ownerPawn) return;

	AController* instigatorController = ownerPawn->GetController();


	const USkeletalMeshSocket* muzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (muzzleFlashSocket)
	{
		FTransform socketTransform = muzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector beginLocation = socketTransform.GetLocation();
		
		for (uint32 pellet = 0; pellet < NumberOfPellets; ++pellet)
		{
			FVector endLocation = TraceEndWithScatter(beginLocation, _hitTarget);
		}
	}
}

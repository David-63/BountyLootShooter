// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Bounty/Character/BountyCharacter.h"

void AHitScanWeapon::Fire(const FVector& _hitTarget)
{
	Super::Fire(_hitTarget);

	APawn* ownerPawn = Cast<APawn>(GetOwner());
	if (nullptr == ownerPawn) return;

	AController* instigatorController = ownerPawn->GetController();


	const USkeletalMeshSocket* muzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (muzzleFlashSocket && instigatorController)
	{
		FTransform socketTransform = muzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector beginLocation = socketTransform.GetLocation();
		FVector endLocation = beginLocation + (_hitTarget - beginLocation) * 1.25f;

		FHitResult bulletHit;
		UWorld* world = GetWorld();
		if (world)
		{
			world->LineTraceSingleByChannel(bulletHit, beginLocation, endLocation, ECollisionChannel::ECC_Visibility);
			if (bulletHit.bBlockingHit)
			{
				ABountyCharacter* victimCharacter = Cast<ABountyCharacter>(bulletHit.GetActor());
				if (victimCharacter)
				{
					if (HasAuthority())
					{
						UGameplayStatics::ApplyDamage(victimCharacter, Damage, instigatorController, this, UDamageType::StaticClass());
					}
				}
				if (ImpactParticle)
				{
					UGameplayStatics::SpawnEmitterAtLocation(world, ImpactParticle, bulletHit.ImpactPoint, bulletHit.ImpactNormal.Rotation());
				}
			}
		}
	}
}

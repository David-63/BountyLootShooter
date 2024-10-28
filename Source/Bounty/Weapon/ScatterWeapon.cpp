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
	UWorld* world = GetWorld();
	if (nullptr == world) return;
	AController* instigatorController = ownerPawn->GetController();


	const USkeletalMeshSocket* muzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (muzzleFlashSocket)
	{
		FTransform socketTransform = muzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector beginLocation = socketTransform.GetLocation();
		
		uint32 hits = 0;
		TMap<ABountyCharacter*, uint32> hitMap;

		for (uint32 pellet = 0; pellet < NumberOfPellets; ++pellet)
		{
			FHitResult bulletHit;


			WeaponTraceHit(beginLocation, _hitTarget, bulletHit);
			FVector endLocation = TraceEndWithScatter(beginLocation, _hitTarget);

			ABountyCharacter* victimCharacter = Cast<ABountyCharacter>(bulletHit.GetActor());
			if (victimCharacter && HasAuthority() && instigatorController)
			{
				if (hitMap.Contains(victimCharacter))
				{
					hitMap[victimCharacter]++;
				}
				else
				{
					hitMap.Emplace(victimCharacter, 1);
				}
				
			}
			if (bulletHit.bBlockingHit)
			{
				if (ImpactParticle)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, bulletHit.ImpactPoint, bulletHit.ImpactNormal.Rotation());
				}
				if (HitSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, HitSound, bulletHit.ImpactPoint, 0.5f, FMath::FRandRange(-.5f, .5f));
				}
			}
		}
		
		for (auto hitPair : hitMap)
		{
			if (hitPair.Key && HasAuthority() && instigatorController)
			{
				UGameplayStatics::ApplyDamage(hitPair.Key, Damage * hitPair.Value, instigatorController, this, UDamageType::StaticClass());
			}

		}
	}
}

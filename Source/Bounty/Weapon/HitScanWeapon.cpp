// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Bounty/Character/BountyCharacter.h"
#include "WeaponTypes.h"
#include "DrawDebugHelpers.h"

void AHitScanWeapon::Fire(const FVector& _hitTarget)
{
	Super::Fire(_hitTarget);

	APawn* ownerPawn = Cast<APawn>(GetOwner());
	if (nullptr == ownerPawn) return;

	AController* instigatorController = ownerPawn->GetController();
	

	const USkeletalMeshSocket* muzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (muzzleFlashSocket)
	{
		FTransform socketTransform = muzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector beginLocation = socketTransform.GetLocation();
		FVector endLocation = beginLocation + (_hitTarget - beginLocation) * 1.25f;

		FHitResult bulletHit;
		UWorld* world = GetWorld();
		if (world)
		{
			world->LineTraceSingleByChannel(bulletHit, beginLocation, endLocation, ECollisionChannel::ECC_Visibility);

			FVector beamEnd = endLocation;
			if (bulletHit.bBlockingHit)
			{
				beamEnd = bulletHit.ImpactPoint;
				ABountyCharacter* victimCharacter = Cast<ABountyCharacter>(bulletHit.GetActor());
				if (victimCharacter && HasAuthority() && instigatorController)
				{
					UGameplayStatics::ApplyDamage(victimCharacter, Damage, instigatorController, this, UDamageType::StaticClass());
				}
				if (ImpactParticle)
				{
					UGameplayStatics::SpawnEmitterAtLocation(world, ImpactParticle, bulletHit.ImpactPoint, bulletHit.ImpactNormal.Rotation());
				}
				if (HitSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, HitSound, bulletHit.ImpactPoint);
				}
			}
			if (BeamParticle)
			{
				UParticleSystemComponent* beam = UGameplayStatics::SpawnEmitterAtLocation(world, BeamParticle, socketTransform);
				if (beam)
				{
					beam->SetVectorParameter(FName("Target"), beamEnd);
				}				
			}
		}

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(world, MuzzleFlash, socketTransform);			
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
	}
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget)
{
	FVector toTargetNormalized = (_hitTarget - _traceStart).GetSafeNormal();

	FVector scatterCenter = _traceStart + toTargetNormalized * EffectiveRange;
	DrawDebugSphere(GetWorld(), scatterCenter, ScatterRadius, 24, FColor::Green, true);

	FVector randVector = UKismetMathLibrary::RandomUnitVector() * FMath::RandRange(0.f, ScatterRadius);
	FVector endLocation = scatterCenter + randVector;
	FVector toEndLocation = endLocation - _traceStart;
	DrawDebugSphere(GetWorld(), endLocation, 4.f, 24, FColor::Orange, true);
	DrawDebugLine(GetWorld(), _traceStart, FVector(_traceStart + toEndLocation * TRACE_LENGTH / toEndLocation.Size()), FColor::Cyan, true);
	return FVector(_traceStart + toEndLocation * TRACE_LENGTH / toEndLocation.Size());
}

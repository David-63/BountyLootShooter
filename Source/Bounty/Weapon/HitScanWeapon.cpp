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
	UWorld* world = GetWorld();
	if (nullptr == world) return;

	AController* instigatorController = ownerPawn->GetController();
	

	const USkeletalMeshSocket* muzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (muzzleFlashSocket)
	{
		FTransform socketTransform = muzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector beginLocation = socketTransform.GetLocation();

		FHitResult bulletHit;
		WeaponTraceHit(beginLocation, _hitTarget, bulletHit);
		
		ABountyCharacter* victimCharacter = Cast<ABountyCharacter>(bulletHit.GetActor());
		if (victimCharacter && HasAuthority() && instigatorController)
		{
			UGameplayStatics::ApplyDamage(victimCharacter, Damage, instigatorController, this, UDamageType::StaticClass());
		}		
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(world, MuzzleFlash, socketTransform);			
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
		if (bulletHit.bBlockingHit)
		{
			if (ImpactParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(world, ImpactParticle, bulletHit.ImpactPoint, bulletHit.ImpactNormal.Rotation());
			}
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, HitSound, bulletHit.ImpactPoint);
			}
		}
	}
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget)
{
	// Ÿ�� ����
	FVector toTargetNormalized = (_hitTarget - _traceStart).GetSafeNormal();
	// ��ȿ�Ÿ��� ��ġ�� ���ع���
	FVector scatterRangeCenter = _traceStart + toTargetNormalized * EffectiveRange;

	// ������ ���� ����
	FVector randomScatterVector = UKismetMathLibrary::RandomUnitVector() * FMath::RandRange(0.f, ScatterRadius);
	// ��� �߽ɿ� ���� ���͸� ���Ͽ� ���� ��ġ ���
	FVector bulletImpactPoint = scatterRangeCenter + randomScatterVector;
	// ���������� ���� ��ġ���� ���� ���
	FVector toEndVector = bulletImpactPoint - _traceStart;

	/*DrawDebugSphere(GetWorld(), scatterRangeCenter, ScatterRadius, 10, FColor::Green, true);
	DrawDebugSphere(GetWorld(), bulletImpactPoint , 4.f, 6, FColor::Orange, true);
	DrawDebugLine(GetWorld(), _traceStart, FVector(_traceStart + toEndVector  * TRACE_LENGTH / toEndVector .Size()), FColor::Cyan, true);*/
	
	
	return FVector(_traceStart + toEndVector * TRACE_LENGTH / toEndVector.Size());
}

void AHitScanWeapon::WeaponTraceHit(const FVector& _traceStart, const FVector& _hitTarget, FHitResult& _inOutHit)
{
	UWorld* world = GetWorld();
	if (world)
	{
		FVector endLocation = bUseScatter ? TraceEndWithScatter(_traceStart, _hitTarget) : _traceStart + (_hitTarget - _traceStart) * 1.25f;
		world->LineTraceSingleByChannel(_inOutHit, _traceStart, endLocation, ECollisionChannel::ECC_Visibility);

		FVector beamEnd = endLocation;
		if (_inOutHit.bBlockingHit)
		{
			beamEnd = _inOutHit.ImpactPoint;
			
		}
		if (BeamParticle)
		{
			UParticleSystemComponent* beam = UGameplayStatics::SpawnEmitterAtLocation(world, BeamParticle, _traceStart, FRotator::ZeroRotator, true);
			if (beam)
			{
				beam->SetVectorParameter(FName("Target"), beamEnd);
			}
		}
		
	}

}

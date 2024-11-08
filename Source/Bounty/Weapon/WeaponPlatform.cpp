// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponPlatform.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Bounty/Character/BountyCharacter.h"
#include "Particles/ParticleSystemComponent.h"

void AWeaponPlatform::FireRound(const FVector& _hitTarget)
{
	// �ּ� ����
	APawn* ownerPawn = Cast<APawn>(GetOwner());
	if (nullptr == ownerPawn) return;
	UWorld* world = GetWorld();
	if (nullptr == world) return;

	// ����Ʈ ������ ���
    PlayWeaponEffect();
	// �Ѿ� �Ҹ��
	SpendRound();

	AController* instigatorController = ownerPawn->GetController();

	if (MuzzleFlashSocket)
	{
		// �ѱ� ��ġ ã��
		FTransform socketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector beginLocation = socketTransform.GetLocation();

		// ��Ƽ�� ���� ���
		if (1 < NumberOfPellets)
		uint32 hits = 0;
		TMap<ABountyCharacter*, uint32> hitMap;
		if (bIsHitScan)
		{
			for (uint32 pellet = 0; pellet < NumberOfPellets; ++pellet)
			{
				FHitResult hitInfo;

				
				FVector endLocation = WeaponTraceHit(beginLocation, _hitTarget, hitInfo);
				FVector trailEnd = endLocation;
				if (hitInfo.bBlockingHit)
				{
					trailEnd = hitInfo.ImpactPoint;

				}
				if (HitScanTrail)
				{
					UParticleSystemComponent* trail = UGameplayStatics::SpawnEmitterAtLocation(world, HitScanTrail, beginLocation, FRotator::ZeroRotator, true);
					if (trail)
					{
						trail->SetVectorParameter(FName("Target"), trailEnd);
					}
				}
				ABountyCharacter* victimCharacter = Cast<ABountyCharacter>(hitInfo.GetActor());
				// ����� ������ TMap�� ���
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
				if (hitInfo.bBlockingHit)
				{
					// impact!!
				}
			}

			for (auto hitPair : hitMap)
			{
				if (hitPair.Key && HasAuthority() && instigatorController)
				{
					UGameplayStatics::ApplyDamage(hitPair.Key, PlatformDamage * hitPair.Value, instigatorController, this, UDamageType::StaticClass());
				}

			}
		}
		else
		{
			// ����ü ����
		}
	}

}

void AWeaponPlatform::PlayWeaponEffect()
{
	APawn* ownerPawn = Cast<APawn>(GetOwner());
	if (nullptr == ownerPawn) return;
	UWorld* world = GetWorld();
	if (nullptr == world) return;

	if (PlatformFireAnimation)
	{
		WeaponMesh->PlayAnimation(PlatformFireAnimation, false);
	}
	else
	{		
		if (MuzzleFlash && MuzzleFlashSocket)
		{
			FTransform socketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
			UGameplayStatics::SpawnEmitterAtLocation(world, MuzzleFlash, socketTransform);
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
	}	
}

FVector AWeaponPlatform::TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget)
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

	//DrawDebugSphere(GetWorld(), scatterRangeCenter, ScatterRadius, 10, FColor::Green, true);
	//DrawDebugSphere(GetWorld(), bulletImpactPoint , 4.f, 6, FColor::Orange, true);
	//DrawDebugLine(GetWorld(), _traceStart, FVector(_traceStart + toEndVector  * TRACE_LENGTH / toEndVector .Size()), FColor::Cyan, true);

	return FVector(_traceStart + toEndVector * TRACE_LENGTH / toEndVector.Size());
}

FVector AWeaponPlatform::WeaponTraceHit(const FVector& _traceStart, const FVector& _hitTarget, FHitResult& _inOutHit)
{
	UWorld* world = GetWorld();
	if (world)
	{
		FVector endLocation = bUseScatter ? TraceEndWithScatter(_traceStart, _hitTarget) : _traceStart + (_hitTarget - _traceStart) * 1.25f;
		world->LineTraceSingleByChannel(_inOutHit, _traceStart, endLocation, ECollisionChannel::ECC_Visibility);
		return endLocation;
	}
	return FVector();
}

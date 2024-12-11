// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponPlatform.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"

#include "Bounty/Character/BountyCharacter.h"
#include "Projectile.h"



void AWeaponPlatform::FireRound(const FVector& _hitTarget)
{
	// 최소 조건
	APawn* ownerPawn = Cast<APawn>(GetOwner());
	if (nullptr == ownerPawn) return;
	UWorld* world = GetWorld();
	if (nullptr == world) return;

	// 이펙트 있으면 재생
    PlayFireEffect(*world);
	// 총알 소모됨
	ChamberingRound();
	AController* instigatorController = ownerPawn->GetController();

	if (MuzzleFlashSocket)
	{
		// 총구 위치 찾기
		FTransform socketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector beginLocation = socketTransform.GetLocation();
		TMap<ABountyCharacter*, uint32> hitMap;
		if (bUsingHitScan)
		{
			FireHitscan(beginLocation, _hitTarget, *world, instigatorController, hitMap);
		}
		else
		{
			FireProjectile(beginLocation, _hitTarget, *world);
		}
	}
}

void AWeaponPlatform::PlayFireEffect(UWorld& _world)
{
	if (PlatformFireAnimation)
	{
		WeaponMesh->PlayAnimation(PlatformFireAnimation, false);
	}
	else
	{
		if (MuzzleFlash && MuzzleFlashSocket)
		{
			FTransform socketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
			UGameplayStatics::SpawnEmitterAtLocation(&_world, MuzzleFlash, socketTransform);

		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
	}
}

void AWeaponPlatform::PlayImpactEffect(FHitResult& _bulletHit, const UWorld& _world)
{
	if (_bulletHit.bBlockingHit)
	{
		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(&_world, ImpactParticles, _bulletHit.ImpactPoint, _bulletHit.ImpactNormal.Rotation());
		}
		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, _bulletHit.ImpactPoint);
		}
	}
}

void AWeaponPlatform::FireHitscan(FVector& beginLocation, const FVector& _hitTarget, const UWorld& world, AController* instigatorController, TMap<ABountyCharacter*, uint32>& hitMap)
{
	for (uint32 pellet = 0; pellet < NumberOfPellets; ++pellet)
	{
		FHitResult hitInfo;
		FVector endLocation = WeaponTraceHit(beginLocation, _hitTarget, hitInfo);

		if (HitScanTrail)
		{
			UParticleSystemComponent* trail = UGameplayStatics::SpawnEmitterAtLocation(&world, HitScanTrail, beginLocation, FRotator::ZeroRotator, true);
			if (trail)
			{
				trail->SetVectorParameter(FName("Target"), endLocation);
			}
		}
		ABountyCharacter* victimCharacter = Cast<ABountyCharacter>(hitInfo.GetActor());
		// 맞춘거 같으면 TMap에 등록
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
			PlayImpactEffect(hitInfo, world);
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
void AWeaponPlatform::FireProjectile(FVector& beginLocation, const FVector& _hitTarget, UWorld& world)
{
	if (AmmoClass)
	{
		APawn* instigatorPawn = Cast<APawn>(GetOwner());
		for (uint32 pellet = 0; pellet < NumberOfPellets; ++pellet)
		{
			FHitResult hitInfo;
			FVector endLocation = bUsingScatter ? TraceEndWithScatter(beginLocation, _hitTarget) : beginLocation + (_hitTarget - beginLocation) * 1.25f;
			FVector toTarget = endLocation - beginLocation;
			FRotator targetRotation = toTarget.Rotation();
			FActorSpawnParameters spawnParams;
			spawnParams.Owner = GetOwner();
			spawnParams.Instigator = instigatorPawn;
			world.SpawnActor<AProjectile>(AmmoClass, beginLocation, targetRotation, spawnParams);
		}
	}
}


FVector AWeaponPlatform::TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget)
{
	// 타겟 방향
	FVector toTargetNormalized = (_hitTarget - _traceStart).GetSafeNormal();
	// 유효거리에 위치한 조준방향
	FVector scatterRangeCenter = _traceStart + toTargetNormalized * EffectiveRange;

	// 무작위 방향 벡터
	FVector randomScatterVector = UKismetMathLibrary::RandomUnitVector() * FMath::RandRange(0.f, ScatterRadius);
	// 산란 중심에 랜덤 벡터를 더하여 최종 위치 계산
	FVector bulletImpactPoint = scatterRangeCenter + randomScatterVector;
	// 시작점에서 최종 위치로의 벡터 계산
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
		FVector endLocation = bUsingScatter ? TraceEndWithScatter(_traceStart, _hitTarget) : _traceStart + (_hitTarget - _traceStart) * 1.25f;
		world->LineTraceSingleByChannel(_inOutHit, _traceStart, endLocation, ECollisionChannel::ECC_Visibility);

		if (_inOutHit.bBlockingHit)
		{
			endLocation = _inOutHit.ImpactPoint;
		}
		return endLocation;
	}
	return FVector();
}

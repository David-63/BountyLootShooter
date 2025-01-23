// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "BountyShooter/Character/ShooterCharacter.h"
#include "BountyShooter/Character/ShooterInventoryHandler.h"
#include "BountyShooter/Character/ShooterCombatHandler.h"

#include "Engine/SkeletalMeshSocket.h"
#include "../ItemMeshComponent.h"
#include "WeaponBase.h"
#include "AmmunitionComponent.h"


// Sets default values for this component's properties
UPlatformComponent::UPlatformComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UPlatformComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPlatformComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlatformComponent::Initialize(AWeaponBase* Base)
{
	WeaponBase = Base;
}

void UPlatformComponent::FireHitscan(const FVector& _hitTarget, AController* InstigatorController)
{
	if (nullptr == InstigatorController) { UE_LOG(LogTemp, Warning, TEXT("Instigator controller is null.")); return; }

	// 사격음
	UGameplayStatics::PlaySoundAtLocation(this, WeaponBase->GetAmmunitionComponent()->FireSound, WeaponBase->GetItemMeshComponent()->GetBoneLocation(FName("Barrel")));

	// Hit 로직
	FVector beginLocation = WeaponBase->GetItemMeshComponent()->GetBoneLocation(FName("Barrel"));
	FVector endLocation = FVector();
	TMap<AActor*, uint32> hitMap;
	FHitResult hitInfo;
	bool isHit = false;
	for (uint32 pellet = 0; pellet < PelletCount; ++pellet)
	{		
		endLocation = WeaponTraceHit(beginLocation, _hitTarget, hitInfo);

		// 궤적 이팩트 재생 (탄약에서 해줘야함)
		WeaponBase->GetAmmunitionComponent()->PlayTrailParticle(endLocation);

		
		AActor* victimActor = Cast<AActor>(hitInfo.GetActor());
		// 맞춘거 같으면 TMap에 등록
		if (victimActor)
		{			
			if (hitMap.Contains(victimActor))
			{
				hitMap[victimActor]++;
			}
			else
			{
				hitMap.Emplace(victimActor, 1);
			}
		}

		// 타격 이팩트 재생 (탄약에서 해줘야함)
		if (hitInfo.bBlockingHit)
		{
			isHit = true;
			WeaponBase->GetAmmunitionComponent()->PlayImpactParticle(endLocation);
		}
	}

	for (auto hitPair : hitMap)
	{
		if (hitPair.Key)
		{
			UGameplayStatics::ApplyDamage(hitPair.Key, GetTotalDamage() * hitPair.Value, InstigatorController, WeaponBase, UDamageType::StaticClass());
		}
	}
	if (isHit)
	{
		if (nullptr == hitInfo.PhysMaterial)
		{
			UE_LOG(LogTemp, Warning, TEXT("No PhysMtrl"));
			return;
		}
		if (EPhysicalSurface::SurfaceType_Default == hitInfo.PhysMaterial->SurfaceType)
		{
			if (WeaponBase->ShooterCharacter->CombatHandler->ImpactPlasterSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, WeaponBase->ShooterCharacter->CombatHandler->ImpactPlasterSound, hitInfo.ImpactPoint);
			}
			if (WeaponBase->ShooterCharacter->CombatHandler->ImpactPlasterDebrisSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, WeaponBase->ShooterCharacter->CombatHandler->ImpactPlasterDebrisSound, hitInfo.ImpactPoint);
			}
		}
		if (EPhysicalSurface::SurfaceType1 == hitInfo.PhysMaterial->SurfaceType)
		{
			if (WeaponBase->ShooterCharacter->CombatHandler->ImpactGlassSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, WeaponBase->ShooterCharacter->CombatHandler->ImpactGlassSound, hitInfo.ImpactPoint);
			}
			if (WeaponBase->ShooterCharacter->CombatHandler->ImpactGlassDebrisSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, WeaponBase->ShooterCharacter->CombatHandler->ImpactGlassDebrisSound, hitInfo.ImpactPoint);
			}
		}
	}
}

FVector UPlatformComponent::WeaponTraceHit(const FVector& _traceStart, const FVector& _hitTarget, FHitResult& _inOutHit)
{
	UWorld* world = GetWorld();
	if (world)
	{
		FVector endLocation = TraceEndWithScatter(_traceStart, _hitTarget);
		FCollisionQueryParams queryParams;
		queryParams.bReturnPhysicalMaterial = true;
		world->LineTraceSingleByChannel(_inOutHit, _traceStart, endLocation, ECollisionChannel::ECC_Visibility, queryParams);

		if (_inOutHit.bBlockingHit)
		{
			endLocation = _inOutHit.ImpactPoint;

			// 충돌지점
			DrawDebugSphere(GetWorld(), endLocation, 4.f, 6, FColor::Magenta, false, 0.1f);
		}
		return endLocation;
	}
	return FVector();
}

FVector UPlatformComponent::TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget)
{
	// 타겟 방향
	FVector toTargetNormalized = (_hitTarget - _traceStart).GetSafeNormal();
	// 유효거리에 위치한 조준방향
	FVector scatterRangeCenter = _traceStart + toTargetNormalized * EffectiveRange;

	// 무작위 방향 벡터
	FVector randomScatterVector = UKismetMathLibrary::RandomUnitVector() * FMath::RandRange(0.f, SpreadRadius);
	// 산란 중심에 랜덤 벡터를 더하여 최종 위치 계산
	FVector bulletImpactPoint = scatterRangeCenter + randomScatterVector;
	// 시작점에서 최종 위치로의 벡터 계산
	FVector toEndVector = bulletImpactPoint - _traceStart;

	// 범위
	DrawDebugSphere(GetWorld(), scatterRangeCenter, SpreadRadius, 16, FColor::Green, false, -1.f);
	// 산탄지점
	DrawDebugSphere(GetWorld(), bulletImpactPoint, 8.f, 6, FColor::Orange, false, 0.1f);
	// 궤적
	DrawDebugLine(GetWorld(), _traceStart, FVector(_traceStart + toEndVector * TRACE_LENGTH / toEndVector.Size()), FColor::Cyan, false, -1.f);
	
	return FVector(_traceStart + toEndVector * TRACE_LENGTH / toEndVector.Size());
}

void UPlatformComponent::CycleCartridge(UWorld* World)
{
	// 탄피 배출 여부
	if (bChamberLoaded)
	{
		AmmoEjectCase(World);
	}

	WeaponBase->GetAmmunitionComponent()->UpdateChamber(CurrentClipSize, PlatformBaseClipCapacity, bChamberLoaded);
		
	if (bChamberLoaded)
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentClip %d + 1"), CurrentClipSize);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentClip %d"), CurrentClipSize);	
	}
}

void UPlatformComponent::AmmoEjectCase(UWorld* World)
{
	if (bChamberLoaded && WeaponBase->GetItemMeshComponent())
	{
		const USkeletalMeshSocket* ejectSocket = WeaponBase->GetItemMeshComponent()->GetSocketByName(FName("Eject"));
		if (ejectSocket)
		{
			WeaponBase->GetAmmunitionComponent()->EjectCase(ejectSocket, World);
		}
	}
}

float UPlatformComponent::GetTotalDamage()
{
	return PlatformBaseDamage;
}

void UPlatformComponent::ChamberingRound()
{
}

void UPlatformComponent::AmmoInsertion()
{
	CurrentClipSize = FMath::Clamp(CurrentClipSize + PlatformBaseClipCapacity, 0, PlatformBaseClipCapacity);
}

void UPlatformComponent::PlayFireMontage()
{
	WeaponBase->GetItemMeshComponent()->PlayAnimation(WeaponFireMontage, false);
}

void UPlatformComponent::PlayReloadMontage()
{
	WeaponBase->GetItemMeshComponent()->PlayAnimation(WeaponReloadMontage, false);
}

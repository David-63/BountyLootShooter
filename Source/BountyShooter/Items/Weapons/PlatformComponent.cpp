// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "BountyShooter/Character/ShooterCharacter.h"
#include "BountyShooter/Character/ShooterInventoryHandler.h"

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

void UPlatformComponent::FireHitscan(UWorld* World, const FVector& _hitTarget, AController* InstigatorController, const USkeletalMeshSocket* MuzzleSocket)
{
	UWorld* world = GetWorld();
	if (nullptr == world) { UE_LOG(LogTemp, Warning, TEXT("World is null.")); return; }
	if (nullptr == InstigatorController) { UE_LOG(LogTemp, Warning, TEXT("Instigator controller is null.")); return; }

	FTransform socketTransform = MuzzleSocket->GetSocketTransform(WeaponBase->GetItemMeshComponent());
	FVector beginLocation = socketTransform.GetLocation();
	TMap<AActor*, uint32> hitMap;
	for (uint32 pellet = 0; pellet < PelletCount; ++pellet)
	{
		FHitResult hitInfo;
		FVector endLocation = WeaponTraceHit(beginLocation, _hitTarget, hitInfo);

		// ���� ����Ʈ ��� (ź�࿡�� �������)
		WeaponBase->GetAmmunitionComponent()->PlayTrailParticle();

		/*if (HitScanTrail)
		{
			UParticleSystemComponent* trail = UGameplayStatics::SpawnEmitterAtLocation(&world, HitScanTrail, beginLocation, FRotator::ZeroRotator, true);
			if (trail)
			{
				trail->SetVectorParameter(FName("Target"), endLocation);
			}
		}*/
		AActor* victimActor = Cast<AActor>(hitInfo.GetActor());
		// ����� ������ TMap�� ���
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

		// Ÿ�� ����Ʈ ��� (ź�࿡�� �������)
		WeaponBase->GetAmmunitionComponent()->PlayFireParticle();
		/*if (hitInfo.bBlockingHit)
		{
			PlayImpactEffect(hitInfo, world);
		}*/
	}

	for (auto hitPair : hitMap)
	{
		if (hitPair.Key)
		{
			UGameplayStatics::ApplyDamage(hitPair.Key, GetTotalDamage() * hitPair.Value, InstigatorController, WeaponBase, UDamageType::StaticClass());
		}
	}
}

FVector UPlatformComponent::WeaponTraceHit(const FVector& _traceStart, const FVector& _hitTarget, FHitResult& _inOutHit)
{
	UWorld* world = GetWorld();
	if (world)
	{
		FVector endLocation = TraceEndWithScatter(_traceStart, _hitTarget);
		world->LineTraceSingleByChannel(_inOutHit, _traceStart, endLocation, ECollisionChannel::ECC_Visibility);

		if (_inOutHit.bBlockingHit)
		{
			endLocation = _inOutHit.ImpactPoint;
		}
		return endLocation;
	}
	return FVector();
}

FVector UPlatformComponent::TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget)
{
	// Ÿ�� ����
	FVector toTargetNormalized = (_hitTarget - _traceStart).GetSafeNormal();
	// ��ȿ�Ÿ��� ��ġ�� ���ع���
	FVector scatterRangeCenter = _traceStart + toTargetNormalized * EffectiveRange;

	// ������ ���� ����
	FVector randomScatterVector = UKismetMathLibrary::RandomUnitVector() * FMath::RandRange(0.f, SpreadRadius);
	// ��� �߽ɿ� ���� ���͸� ���Ͽ� ���� ��ġ ���
	FVector bulletImpactPoint = scatterRangeCenter + randomScatterVector;
	// ���������� ���� ��ġ���� ���� ���
	FVector toEndVector = bulletImpactPoint - _traceStart;

	DrawDebugSphere(GetWorld(), scatterRangeCenter, SpreadRadius, 10, FColor::Green, false, 0.1f);
	DrawDebugSphere(GetWorld(), bulletImpactPoint, 4.f, 6, FColor::Orange, false, 0.1f);
	DrawDebugLine(GetWorld(), _traceStart, FVector(_traceStart + toEndVector * TRACE_LENGTH / toEndVector.Size()), FColor::Cyan, false, 0.1f);

	return FVector(_traceStart + toEndVector * TRACE_LENGTH / toEndVector.Size());
}

void UPlatformComponent::CycleCartridge(UWorld* World)
{
	if (bChamberLoaded && WeaponBase->GetItemMeshComponent())
	{
		const USkeletalMeshSocket* ejectSocket = WeaponBase->GetItemMeshComponent()->GetSocketByName(FName("Eject"));
		if (ejectSocket)
		{
			WeaponBase->GetAmmunitionComponent()->EjectCase(ejectSocket, World);
		}
	}

	WeaponBase->GetAmmunitionComponent()->UpdateChamber(ClipSize, ClipCapacity, bChamberLoaded);
}

float UPlatformComponent::GetTotalDamage()
{
	return PlatformBaseDamage;
}


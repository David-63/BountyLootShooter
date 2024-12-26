// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlatformComponent.generated.h"


class AWeaponBase;

UCLASS( Blueprintable, BlueprintType, ClassGroup = (Weapon), meta=(BlueprintSpawnableComponent) )
class BOUNTYSHOOTER_API UPlatformComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlatformComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Initialize(AWeaponBase* Base);

private:
	TObjectPtr<AWeaponBase> WeaponBase = nullptr;



private:
	UPROPERTY(EditAnywhere, Category = "Platform Property")	// ��ȿ �Ÿ�
		float EffectiveRange = 500.f;
	UPROPERTY(EditAnywhere, Category = "Platform Property")	// ��ź(��ź��) ����
		float SpreadRadius = 25.f;
	UPROPERTY(EditAnywhere, Category = "Platform Property") // ��Ƽ�� ����
		uint32 PelletCount = 2;

	UPROPERTY(EditAnywhere, Category = "Platform Property")
	float PlatformBaseDamage = 5.f;


	UPROPERTY(EditAnywhere, Category = "Platform Property")
	uint32 ClipCapacity = 25.f;
	UPROPERTY(EditAnywhere, Category = "Platform Property")
	uint32 ClipSize = 25.f;

	bool bChamberLoaded = true;
public:
	void FireHitscan(UWorld* World, const FVector& _hitTarget, AController* InstigatorController, const USkeletalMeshSocket* MuzzleSocket);
	FVector WeaponTraceHit(const FVector& _traceStart, const FVector& _hitTarget, FHitResult& _inOutHit);
	FVector TraceEndWithScatter(const FVector& _traceStart, const FVector& _hitTarget);
	void CycleCartridge(UWorld* World);

	float GetTotalDamage();
		
};

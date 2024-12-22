// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Bounty/Bounty.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);

	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CasingMesh->SetCollisionResponseToChannel(ECC_SkeletalMesh , ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();
	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	SetLifeSpan(2.5f);

	AActor* owner = GetOwner();
	if (owner)
	{
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(GetOwner());
		CasingMesh->IgnoreActorWhenMoving(GetOwner(), true);
	}
}

void ACasing::OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}
	CasingMesh->SetNotifyRigidBodyCollision(false);
	
}

void ACasing::CasingImpulse()
{
	// 기본 이동량
	FVector casingDirection = GetActorForwardVector() * 1.9 + GetActorRightVector() * 1.6;
	casingDirection += FMath::VRand() * 1.5f; // 약간의 랜덤 벡터 추가
	casingDirection = casingDirection.GetSafeNormal();
	FVector ejectionPower = casingDirection * ShellEjectionImpulse;

	CasingMesh->AddImpulse(ejectionPower);

	FVector randomSpin = FMath::VRand() * 3.0f; // 임의의 회전력 추가
	CasingMesh->AddTorqueInRadians(randomSpin);
}

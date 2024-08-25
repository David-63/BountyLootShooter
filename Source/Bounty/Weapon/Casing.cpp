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
	CasingMesh->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();
	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	SetLifeSpan(2.f);

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
	FVector ejectionPower = GetActorForwardVector() * ShellEjectionImpulse;
	//FVector velocity = OwnerVelocity;
	//velocity.Z = 0;
	//velocity.Normalize();
	//
	//float dotDiraction = FVector::DotProduct(GetActorForwardVector(), velocity);
	//float impulseScalingFactor = FMath::Clamp(dotDiraction, 0.0f, 1.0f);
	//ejectionPower += ejectionPower * dotDiraction * 2.f;

	CasingMesh->AddImpulse(ejectionPower);
}

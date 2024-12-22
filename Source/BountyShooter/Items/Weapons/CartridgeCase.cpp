// Fill out your copyright notice in the Description page of Project Settings.


#include "CartridgeCase.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ACartridgeCase::ACartridgeCase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	CartridgeCaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CartridgeCaseMesh);
	CartridgeCaseMesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	CartridgeCaseMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CartridgeCaseMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void ACartridgeCase::BeginPlay()
{
	Super::BeginPlay();
	CartridgeCaseMesh->OnComponentHit.AddDynamic(this, &ACartridgeCase::OnHit);
	SetLifeSpan(2.5f);

	AActor* owner = GetOwner();
	if (owner)
	{
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(GetOwner());
		CartridgeCaseMesh->IgnoreActorWhenMoving(GetOwner(), true);
	}
}

// Called every frame
void ACartridgeCase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACartridgeCase::OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}
	CartridgeCaseMesh->SetNotifyRigidBodyCollision(false);
}

void ACartridgeCase::CasingImpulse()
{
	// 기본 이동량
	FVector casingDirection = GetActorForwardVector() * 1.9 + GetActorRightVector() * 1.6;
	casingDirection += FMath::VRand() * 1.5f; // 약간의 랜덤 벡터 추가
	casingDirection = casingDirection.GetSafeNormal();
	FVector ejectionPower = casingDirection * ShellEjectionImpulse;

	CartridgeCaseMesh->AddImpulse(ejectionPower);

	FVector randomSpin = FMath::VRand() * 3.0f; // 임의의 회전력 추가
	CartridgeCaseMesh->AddTorqueInRadians(randomSpin);
}


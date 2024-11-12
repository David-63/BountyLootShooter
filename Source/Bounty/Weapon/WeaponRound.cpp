// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponRound.h"
#include "Bounty/Bounty.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "ThrusterMovementComponent.h"

#include "Particles/ParticleSystemComponent.h"
#include "NiagaraSystemInstanceController.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"



AWeaponRound::AWeaponRound()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RoundMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Round Mesh"));
	RoundMesh->SetupAttachment(RootComponent);
	RoundMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);


	ThrusterMovementComponent = CreateDefaultSubobject<UThrusterMovementComponent>(TEXT("RocketMovementComponent"));
	ThrusterMovementComponent->Deactivate();

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
}

void AWeaponRound::BeginPlay()
{
	Super::BeginPlay();

	if (BulletTracerSystem)
	{
		BulletTracerComponent = UGameplayStatics::SpawnEmitterAttached(
			BulletTracerSystem, CollisionBox, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
	}

	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}

	if (bUseExplosive)
	{
		if (bUseBounce)
		{
			StartDestroyTimer();

			ProjectileMovementComponent->bShouldBounce = true;
			ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &ThisClass::OnBounce);
		}
		else
		{
			ThrusterMovementComponent->Activate();
			ProjectileMovementComponent->Deactivate();
			SpawnTrailSystem();
			FCollisionQueryParams queryParams;
			queryParams.AddIgnoredActor(GetOwner());
			CollisionBox->IgnoreActorWhenMoving(GetOwner(), true);

			ThrusterMovementComponent->bRotationFollowsVelocity = true;
			ThrusterMovementComponent->SetIsReplicated(true);
			if (ProjectileLoopSound && LoopSoundAttenuation)
			{
				ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(
					ProjectileLoopSound, GetRootComponent(), FName(), GetActorLocation(), EAttachLocation::KeepWorldPosition,
					false, 1.f, 1.f, 0.f, LoopSoundAttenuation, (USoundConcurrency*)nullptr, false);
			}
		}		
	}
}

void AWeaponRound::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponRound::Destroyed()
{
	if (bUseBounce)
	{
		ExplodeDamage();
	}

	Super::Destroyed();
	if (!bUseExplosive)
	{
		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
		}
		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		}
	}
	
}


void AWeaponRound::OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit)
{
	
	if (bUseExplosive)
	{
		if (_otherActor == GetOwner()) return;

		ExplodeDamage();

		StartDestroyTimer(); // 이펙트와 사운드를 재생하기 위해서 투명처리하고 잠시 살림

		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
		}
		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		}
		if (RoundMesh)
		{
			RoundMesh->SetVisibility(false);
		}
		if (CollisionBox)
		{
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		if (ThrusterTrailComponent && ThrusterTrailComponent->GetSystemInstance())
		{
			ThrusterTrailComponent->GetSystemInstanceController()->Deactivate();
		}
		if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
		{
			ProjectileLoopComponent->Stop();
		}
	}
	else
	{
		ACharacter* ownerCharacter = Cast<ACharacter>(GetOwner());
		if (ownerCharacter)
		{
			AController* ownerController = ownerCharacter->Controller;

			if (ownerController)
			{
				UGameplayStatics::ApplyDamage(_otherActor, TotalDamage, ownerController, this, UDamageType::StaticClass());
			}
		}
		Destroy();
	}

}

void AWeaponRound::StartDestroyTimer()
{
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &ThisClass::DestroyTimerFinished, DestroyTime);
}

void AWeaponRound::DestroyTimerFinished()
{
	Destroy();
}

void AWeaponRound::ExplodeDamage()
{
	APawn* firingPawn = GetInstigator();
	if (firingPawn && HasAuthority())
	{
		AController* firingController = firingPawn->GetController();
		if (firingController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,	// world context object
				TotalDamage, // base damage
				TotalDamage / 2.f, // minimum damage
				GetActorLocation(), // origin
				DamageInnerRadius, // damage inner radius
				DamageOuterRadius, // damage outer radius
				1.f, // damage fallof
				UDamageType::StaticClass(), // damage type class
				TArray<AActor*>(), // ignore actors
				this, // damage causer
				firingController // instigator controller
			);
		}
	}
}

void AWeaponRound::OnBounce(const FHitResult& _impactResult, const FVector& _impactVelocity)
{
	if (BounceSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BounceSound, GetActorLocation());
	}
}

void AWeaponRound::SpawnTrailSystem()
{
	if (ThrusterTrailSystem)
	{
		ThrusterTrailComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			ThrusterTrailSystem, GetRootComponent(), FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstance.h"
#include "ThrusterMovementComponent.h"

AProjectileRocket::AProjectileRocket()
{
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ThrusterMovementComponent = CreateDefaultSubobject<UThrusterMovementComponent>(TEXT("RocketMovementComponent"));
	ThrusterMovementComponent->bRotationFollowsVelocity = true;
	ThrusterMovementComponent->SetIsReplicated(true);
}

void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}

	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem, GetRootComponent(), FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
	}
	if (ProjectileLoopSound && LoopSoundAttenuation)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(
			ProjectileLoopSound, GetRootComponent(), FName(), GetActorLocation(), EAttachLocation::KeepWorldPosition,
			false, 1.f, 1.f, 0.f, LoopSoundAttenuation, (USoundConcurrency*)nullptr, false);
	}
}

void AProjectileRocket::Destroyed()
{
}

void AProjectileRocket::OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit)
{
	if (_otherActor == GetOwner()) return;
		
	APawn* firingPawn = GetInstigator();
	if (firingPawn && HasAuthority())
	{
		AController* firingController = firingPawn->GetController();
		if (firingController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,	// world context object
				Damage, // base damage
				Damage / 2.f, // minimum damage
				GetActorLocation(), // origin
				200.f, // damage inner radius
				500.f, // damage outer radius
				1.f, // damage fallof
				UDamageType::StaticClass(), // damage type class
				TArray<AActor*>(), // ignore actors
				this, // damage causer
				firingController // instigator controller
				);
		}
	}

	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AProjectileRocket::DestroyTimerFinished, DestroyTime);
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (RocketMesh)
	{
		RocketMesh->SetVisibility(false);
	}
	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (TrailSystemComponent && TrailSystemComponent->GetSystemInstance())
	{
		TrailSystemComponent->GetSystemInstance()->Deactivate();
	}
	if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}

	//Super::OnHit(_hitComp, _otherActor, _otherComp, _normalImpulse, _hit);
}

void AProjectileRocket::DestroyTimerFinished()
{
	Destroy();
}

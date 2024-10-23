// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"

AProjectileRocket::AProjectileRocket()
{
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProjectileRocket::OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit)
{
	APawn* firingPawn = GetInstigator();
	if (firingPawn)
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

	Super::OnHit(_hitComp, _otherActor, _otherComp, _normalImpulse, _hit);
}

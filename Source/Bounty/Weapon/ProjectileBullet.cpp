// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void AProjectileBullet::OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit)
{
	ACharacter* ownerCharacter = Cast<ACharacter>(GetOwner());
	if (ownerCharacter)
	{
		AController* ownerController = ownerCharacter->Controller;

		if (ownerController)
		{
			UGameplayStatics::ApplyDamage(_otherActor, Damage, ownerController, this, UDamageType::StaticClass());
		}
	}
	

	// edit last
	Super::OnHit(_hitComp, _otherActor, _otherComp, _normalImpulse, _hit);
}

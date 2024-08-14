// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"


void AProjectileWeapon::Fire(const FVector& _hitTarget)
{
	Super::Fire(_hitTarget);
	USkeletalMeshComponent* weaponMesh = GetWeaponMesh();
	const USkeletalMeshSocket* muzzleSocket = weaponMesh->GetSocketByName(FName("MuzzleFlash"));
	if (!muzzleSocket) return;
	
	FTransform socketTransform = muzzleSocket->GetSocketTransform(weaponMesh);

	UWorld* world = GetWorld();
	if (!world) return;


	APawn* instigatorPawn = Cast<APawn>(GetOwner());
	// From muzzleSocket to hit location from traceUnderCrosshair
	FVector toTarget = _hitTarget - socketTransform.GetLocation();
	FRotator targetRotation = toTarget.Rotation();

	if (ProjectileClass && instigatorPawn)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = GetOwner();
		spawnParams.Instigator = instigatorPawn;
		world->SpawnActor<AProjectile>(ProjectileClass, socketTransform.GetLocation(), targetRotation, spawnParams);
	}

}

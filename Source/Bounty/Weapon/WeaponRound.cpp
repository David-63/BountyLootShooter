// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponRound.h"
#include "Components/BoxComponent.h"
#include "Bounty/Bounty.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"
#include "ThrusterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AWeaponRound::AWeaponRound()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AWeaponRound::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponRound::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponRound::Destroyed()
{
	Super::Destroyed();
	
}

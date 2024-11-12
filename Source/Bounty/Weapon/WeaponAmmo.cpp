// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAmmo.h"

UWeaponAmmo::UWeaponAmmo()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UWeaponAmmo::BeginPlay()
{
	Super::BeginPlay();

	
}


void UWeaponAmmo::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


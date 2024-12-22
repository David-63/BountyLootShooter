// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemMeshComponent.h"

UItemMeshComponent::UItemMeshComponent()
{
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UItemMeshComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UItemMeshComponent::ItemCollisionDisable()
{
	SetSimulatePhysics(false);
	SetEnableGravity(false);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UItemMeshComponent::ItemCollisionEnable()
{
	SetSimulatePhysics(true);
	SetEnableGravity(true);
	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

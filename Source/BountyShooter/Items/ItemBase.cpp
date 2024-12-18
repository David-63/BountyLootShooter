// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBase.h"
#include "PickupComponent.h"
#include "ItemMeshComponent.h"

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemBase::SetPicupComponent(UPickupComponent* Pickup)
{
	PickupArea = Pickup;
}

void AItemBase::SetItemMeshComponent(UItemMeshComponent* Mesh)
{
	ItemMeshComponent = Mesh;
}

void AItemBase::ChangeItemState(EItemState State)
{
	switch (State)
	{
	case EItemState::EIS_Initial:
		break;
	case EItemState::EIS_Equipped:
		PickupArea->PickupDisable();
		ItemMeshComponent->ItemCollisionDisable();
		break;
	case EItemState::EIS_Dropped:
		PickupArea->PickupEnable();
		ItemMeshComponent->ItemCollisionEnable();
		break;
	case EItemState::EIS_MAX:
		break;
	default:
		break;
	}
}

void AItemBase::Equip(AShooterCharacter* Owner, FName Socket)
{	
	SetOwner(Owner);
	ChangeItemState(EItemState::EIS_Equipped);
	// Attach or hide
	ItemMeshComponent->SetVisibility(false);
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(ShooterCharacter->GetMesh(), AttachmentRules, Socket);		// 따로 지정하진 않을태니, 발바닥에 붙어있을 것임
}

void AItemBase::Drop()
{
	SetOwner(nullptr);
	ChangeItemState(EItemState::EIS_Equipped);
	ItemMeshComponent->SetVisibility(true);
	// Detach
	FDetachmentTransformRules detachRules(EDetachmentRule::KeepWorld, true);
	ItemMeshComponent->DetachFromComponent(detachRules);
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupComponent.h"
#include "ItemBase.h"


UPickupComponent::UPickupComponent()
{
	SphereRadius = 300.f;

	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void UPickupComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UPickupComponent::OnSphereBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UPickupComponent::OnSphereEndOverlap);
}

void UPickupComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	AShooterCharacter* shooter = Cast<AShooterCharacter>(OtherActor);
	if (shooter != nullptr)
	{
		// shooter 의 overlapItems 배열에 등록
		AItemBase* thisItem = Cast<AItemBase>(GetOwner());

		shooter->SetOverlappingItems(thisItem);
	}
}

void UPickupComponent::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AShooterCharacter* shooter = Cast<AShooterCharacter>(OtherActor);
	if (shooter != nullptr)
	{
		// shooter 의 overlapItems 배열에 해제
		AItemBase* thisItem = Cast<AItemBase>(GetOwner());

		shooter->SetOverlappingItems(thisItem, false);
	}
}

void UPickupComponent::PickupDisable()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UPickupComponent::PickupEnable()
{
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

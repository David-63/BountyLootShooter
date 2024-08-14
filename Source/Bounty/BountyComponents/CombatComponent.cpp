// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Bounty/Character/BountyCharacter.h"
#include "Bounty/Weapon/BaseWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	BaseMoveSpeed = 550.f;
	ADSMoveSpeed = 250.f;
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseMoveSpeed;
	}
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FHitResult traceHitResult;
	TraceUnderCrosshairs(traceHitResult);
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& _traceHitResult)
{
	if (!GEngine && !GEngine->GameViewport) return;

	FVector2D viewportSize;
	GEngine->GameViewport->GetViewportSize(viewportSize);

	FVector2D crossHairLocation(viewportSize.X / 2.f, viewportSize.Y / 2.f);
	FVector crossHairWorldPosition;
	FVector crossHairWorldDirection;
	bool isScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
		(UGameplayStatics::GetPlayerController(this, 0), crossHairLocation, crossHairWorldPosition, crossHairWorldDirection);

	if (isScreenToWorld)
	{
		FVector begin = crossHairWorldPosition;
		FVector end = begin + crossHairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(_traceHitResult, begin, end, ECollisionChannel::ECC_Visibility);

		if (!_traceHitResult.bBlockingHit)
		{
			_traceHitResult.ImpactPoint = end;
			HitTarget = end;
		}
		else
		{
			HitTarget = _traceHitResult.ImpactPoint;
			DrawDebugSphere(GetWorld(), _traceHitResult.ImpactPoint, 12.f, 6, FColor::Emerald);
		}
	}

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bIsADS);
}


void UCombatComponent::OnRep_EquipWeapon()
{
	if (EquippedWeapon && Character)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::Attack(bool _presseed)
{
	bIsAttackHold = _presseed;

	if (bIsAttackHold)
	{
		ServerAttack();
	}
	
}

void UCombatComponent::ServerAttack_Implementation()
{
	MulticastAttack();
}

void UCombatComponent::MulticastAttack_Implementation()
{
	if (!Character) return;
	if (!EquippedWeapon) return;

	GEngine->AddOnScreenDebugMessage(2, 0.1f, FColor::Blue, FString::Printf(TEXT("Attak called by combat")));
	Character->PlayFireArmMontage(bIsADS);
	EquippedWeapon->Fire(HitTarget);
}




void UCombatComponent::EquipWeapon(ABaseWeapon* _weaponToEquip)
{
	if (nullptr == Character || nullptr == _weaponToEquip) return;

	EquippedWeapon = _weaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::ServerSetADS_Implementation(bool _bIsADS)
{
	bIsADS = _bIsADS;
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsADS ? ADSMoveSpeed : BaseMoveSpeed;
	}
}

void UCombatComponent::SetADS(bool _bIsADS)
{
	bIsADS = _bIsADS;
	ServerSetADS(_bIsADS);
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsADS ? ADSMoveSpeed : BaseMoveSpeed;
	}
}
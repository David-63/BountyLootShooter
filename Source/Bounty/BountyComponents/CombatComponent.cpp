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
#include "Bounty/PlayerController/BountyPlayerController.h"
#include "Bounty/HUD/BountyHUD.h"

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
	SetHUDCrosshairs(DeltaTime);
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& _traceHitResult)
{
	if (!GEngine || !GEngine->GameViewport) return;

	FVector2D viewportSize;
	GEngine->GameViewport->GetViewportSize(viewportSize);

	FVector crossHairWorldPosition;
	FVector crossHairWorldDirection;

	FVector2D crossHairLocation(viewportSize.X / 2.f, viewportSize.Y / 2.f);

	bool isScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
		(UGameplayStatics::GetPlayerController(this, 0), crossHairLocation, crossHairWorldPosition, crossHairWorldDirection);

	if (!isScreenToWorld) return;

	FVector begin = crossHairWorldPosition;
	FVector end = begin + crossHairWorldDirection * TRACE_LENGTH;

	GetWorld()->LineTraceSingleByChannel(_traceHitResult, begin, end, ECollisionChannel::ECC_Visibility);

	if (!_traceHitResult.bBlockingHit)
	{
		_traceHitResult.ImpactPoint = end;
	}
	else
	{
		DrawDebugSphere(GetWorld(), _traceHitResult.ImpactPoint, 12.f, 6, FColor::Emerald);
	}
}

void UCombatComponent::SetHUDCrosshairs(float _deltaTime)
{
	if (!Character || !Character->Controller) return;

	PlayerController = PlayerController == nullptr ? Cast<ABountyPlayerController>(Character->Controller) : PlayerController;

	HUD = HUD == nullptr ? Cast<ABountyHUD>(PlayerController->GetHUD()) : HUD;
	
	if (!HUD) return;

	FHUDPackage hudPackage;
	if (EquippedWeapon)
	{
		hudPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
		hudPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
		hudPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
		hudPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
		hudPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
	}
	else
	{
		hudPackage.CrosshairsCenter = nullptr;
		hudPackage.CrosshairsLeft = nullptr;
		hudPackage.CrosshairsRight = nullptr;
		hudPackage.CrosshairsTop = nullptr;
		hudPackage.CrosshairsBottom = nullptr;
	}
	
	HUD->SetHUDPackage(hudPackage);

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
		FHitResult traceHitResult;
		TraceUnderCrosshairs(traceHitResult);
		ServerAttack(traceHitResult.ImpactPoint);
	}
	
}

void UCombatComponent::ServerAttack_Implementation(const FVector_NetQuantize& _traceHitTarget)
{
	MulticastAttack(_traceHitTarget);
}

void UCombatComponent::MulticastAttack_Implementation(const FVector_NetQuantize& _traceHitTarget)
{
	if (!Character) return;
	if (!EquippedWeapon) return;

	Character->PlayFireArmMontage(bIsADS);
	EquippedWeapon->Fire(_traceHitTarget);
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
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
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (!Character) return;
	Character->GetCharacterMovement()->MaxWalkSpeed = BaseMoveSpeed;

	if (Character->GetFollowCamera())
	{
		DefaultFOV = Character->GetFollowCamera()->FieldOfView;
		CurrentFOV = DefaultFOV;
	}
	if (Character->HasAuthority())
	{
		InitializeExtraAmmo();
	}
}
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		InterpFov(DeltaTime);
		SetHUDCrosshairs(DeltaTime);

		FHitResult result;
		TraceUnderCrosshairs(result);
		HitTarget = result.ImpactPoint;
		
	}
}
void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bIsADS);
	DOREPLIFETIME_CONDITION(UCombatComponent, ExtraAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, CombatState);
}


void UCombatComponent::TraceUnderCrosshairs(FHitResult& _traceHitResult)
{
	FVector2D viewportSize;	
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(viewportSize);
	}
	
	// center location
	FVector2D crossHairLocation(viewportSize.X / 2.f, viewportSize.Y / 2.f);

	FVector crossHairWorldPosition, crossHairWorldDirection;

	// Increase the size of inertia by multiplying inertiaMagnitude
	FVector2D inertiaValue = Character->GetInertiaValue() * InertiaMagnitude;
	crossHairLocation.X += inertiaValue.X;
	crossHairLocation.Y += inertiaValue.Y;

	// Calculate inverse matrix from the crosshair position with inertia applied | screen -> World
	bool isScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
		(UGameplayStatics::GetPlayerController(this, 0), crossHairLocation, crossHairWorldPosition, crossHairWorldDirection);

	if (!isScreenToWorld) return;

	FVector begin = crossHairWorldPosition;

	if (Character)
	{
		float distanceToCharacter = (Character->GetActorLocation() - begin).Size();
		begin += crossHairWorldDirection * (distanceToCharacter + 100.f);
	}
	FVector end = begin + crossHairWorldDirection * TRACE_LENGTH;
	

	// Ray result return
	GetWorld()->LineTraceSingleByChannel(_traceHitResult, begin, end, ECollisionChannel::ECC_Visibility);

	if (_traceHitResult.GetActor() && _traceHitResult.GetActor()->Implements<UCrosshairInteractor>())
	{
		CrosshairPackage.CrosshairsColor = FLinearColor::Yellow;
	}
	else
	{
		CrosshairPackage.CrosshairsColor = FLinearColor::White;
	}

	if (!_traceHitResult.bBlockingHit)
	{
		_traceHitResult.ImpactPoint = end;
	}

}
void UCombatComponent::SetHUDCrosshairs(float _deltaTime)
{
	if (!Character || !Character->Controller) return;

	PlayerController = PlayerController == nullptr ? Cast<ABountyPlayerController>(Character->Controller) : PlayerController;
	if (!PlayerController) return;
	HUD = HUD == nullptr ? Cast<ABountyHUD>(PlayerController->GetHUD()) : HUD;

	if (EquippedWeapon)
	{
		CrosshairPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
		CrosshairPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
		CrosshairPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
		CrosshairPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;		
		CrosshairPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
	}
	else
	{
		CrosshairPackage.CrosshairsCenter = nullptr;
		CrosshairPackage.CrosshairsLeft = nullptr;
		CrosshairPackage.CrosshairsRight = nullptr;
		CrosshairPackage.CrosshairsTop = nullptr;
		CrosshairPackage.CrosshairsBottom = nullptr;
	}

	// calculate crosshair spread
	// movement spread
	FVector2D maxSpeedRange(0.f, BaseMoveSpeed);
	FVector2D velocityRatioRange(0.f, 1.f);
	FVector currentVelocity = Character->GetVelocity();
	currentVelocity.Z = 0.f;
	float crosshairVelocityFactor = FMath::GetMappedRangeValueClamped(maxSpeedRange, velocityRatioRange, currentVelocity.Size());

	// ADS spread
	if (bIsADS)
	{
		// the last parameter must be set according to the weapon's ADS transition speed
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, -SpreadCorrection, _deltaTime, 20.f);
	}
	else
	{
		// the last parameter must be set according to the weapon's ADS transition speed
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, _deltaTime, 20.f);
	}

	// Fire spread
	CrosshairAttackingFactor = FMath::FInterpTo(CrosshairAttackingFactor, 0.f, _deltaTime, RecoveryMOA);

	// inAir spread
	if (Character->GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.5f, _deltaTime, 2.5f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, _deltaTime, 25.f);
	}

	// total spread (0.2f is default spread)
	CrosshairPackage.SpreadFactor = BaseSpread + crosshairVelocityFactor + CrosshairInAirFactor + CrosshairAimFactor + CrosshairAttackingFactor;

	HUD->SetCrosshairPackage(CrosshairPackage, Character->GetInertiaValue() * InertiaMagnitude);
}


void UCombatComponent::EquipWeapon(ABaseWeapon* _weaponToEquip)
{
	if (nullptr == Character || nullptr == _weaponToEquip) return;

	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}

	EquippedWeapon = _weaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetHUDCurrentAmmo();

	// update weapon types
	if (ExtraAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		ExtraAmmo = ExtraAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	// display weapon ammo
	PlayerController = nullptr == PlayerController ? Cast<ABountyPlayerController>(Character->Controller) : PlayerController;
	if (PlayerController)
	{
		PlayerController->SetHUD_ExtraAmmo(ExtraAmmo);
	}

	if (EquippedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquippedWeapon->EquipSound, Character->GetActorLocation());
	}
	if (EquippedWeapon->IsMagEmpty())
	{
		WeaponReload();
	}

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}
void UCombatComponent::OnRep_EquipWeapon()
{
	if (EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;

		if (EquippedWeapon->EquipSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, EquippedWeapon->EquipSound, Character->GetActorLocation());
		}
	}	
}


void UCombatComponent::Fire()
{	
	if (!EquippedWeapon) return;
	if (CanFire())
	{		
		bCanAttack = false;
		ServerAttack(HitTarget);
		CrosshairAttackingFactor = SpreadMOA;
		StartFireTimer();
	}	
}
void UCombatComponent::StartFireTimer()
{
	if (!EquippedWeapon || !Character) return;

	Character->GetWorldTimerManager().SetTimer(FireTimer, this, &UCombatComponent::FireTimerFinished, EquippedWeapon->FireDelay);
}
void UCombatComponent::FireTimerFinished()
{
	bCanAttack = true;
	if (bIsAttackDown && EquippedWeapon->bUseAutoAttack)
	{
		Fire();
	}
	if (EquippedWeapon->IsMagEmpty())
	{
		WeaponReload();
	}
}
bool UCombatComponent::CanFire()
{
	if (!EquippedWeapon) return false;

	return !EquippedWeapon->IsMagEmpty() && bCanAttack && ECombatState::ECS_Unoccupied == CombatState;
}

void UCombatComponent::WeaponReload()
{
	if (0 < ExtraAmmo && ECombatState::ECS_Reloading != CombatState)
	{
		ServerWeaponReload();
	}
}
void UCombatComponent::WeaponReloadFinish()
{
	if (!Character) return;
	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmoValue();
	}
	if (bIsAttackDown)
	{
		Fire();
	}
}
void UCombatComponent::ServerWeaponReload_Implementation()
{
	if (!Character || !EquippedWeapon) return;
	
	CombatState = ECombatState::ECS_Reloading;
	HandleReload();
}
void UCombatComponent::HandleReload()
{
	Character->PlayReloadMontage();
}
int32 UCombatComponent::AmountToReload()
{
	if (!EquippedWeapon) return 0;

	int32 roomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();
	if (ExtraAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32 amountCarried = ExtraAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 least = FMath::Min(roomInMag, amountCarried);
		return FMath::Clamp(roomInMag, 0, least);
	}

	return 0;
}


void UCombatComponent::OnRep_ExtraAmmo()
{
	PlayerController = nullptr == PlayerController ? Cast<ABountyPlayerController>(Character->Controller) : PlayerController;
	if (PlayerController)
	{
		PlayerController->SetHUD_ExtraAmmo(ExtraAmmo);
	}
}
void UCombatComponent::InitializeExtraAmmo()
{
	ExtraAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingAmmoAR);
	ExtraAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, StartingAmmoRocket);
	ExtraAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingAmmoPistol);
}
void UCombatComponent::UpdateAmmoValue()
{
	if (!Character || !EquippedWeapon) return;

	int32 reloadAmount = AmountToReload();
	if (ExtraAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		ExtraAmmoMap[EquippedWeapon->GetWeaponType()] -= reloadAmount;
		ExtraAmmo = ExtraAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	PlayerController = nullptr == PlayerController ? Cast<ABountyPlayerController>(Character->Controller) : PlayerController;
	if (PlayerController)
	{
		PlayerController->SetHUD_ExtraAmmo(ExtraAmmo);
	}
	EquippedWeapon->AddAmmo(reloadAmount);
	
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Unoccupied:
		if (bIsAttackDown)
		{
			Fire();
		}
		break;
	case ECombatState::ECS_Reloading:
		HandleReload();
		break;
	}
}




void UCombatComponent::Attack(bool _presseed)
{
	bIsAttackDown = _presseed;

	if (!bIsAttackDown) return;

	Fire();
	
}
void UCombatComponent::ServerAttack_Implementation(const FVector_NetQuantize& _traceHitTarget)
{
	MulticastAttack(_traceHitTarget);
}
void UCombatComponent::MulticastAttack_Implementation(const FVector_NetQuantize& _traceHitTarget)
{
	if (!Character) return;
	if (!EquippedWeapon) return;

	if (ECombatState::ECS_Unoccupied == CombatState)
	{
		Character->PlayFireMontage(bIsADS);
		EquippedWeapon->Fire(_traceHitTarget);
	}
}


void UCombatComponent::InterpFov(float _deltaTime)
{
	if (!EquippedWeapon) return;

	if (bIsADS)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), _deltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, _deltaTime, ZoomInterpSpeed);
	}

	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
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
void UCombatComponent::ServerSetADS_Implementation(bool _bIsADS)
{
	bIsADS = _bIsADS;
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsADS ? ADSMoveSpeed : BaseMoveSpeed;
	}
}

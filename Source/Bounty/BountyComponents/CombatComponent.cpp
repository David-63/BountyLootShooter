// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
//#include "DrawDebugHelpers.h"

#include "Bounty/Character/BountyCharacter.h"
#include "Bounty/PlayerController/BountyPlayerController.h"
#include "Bounty/Character/BountyAnimInstance.h"
#include "Bounty/Weapon/BaseWeapon.h"
#include "Bounty/Weapon/Projectile.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (!Character) return;
	Character->GetCharacterMovement()->MaxWalkSpeed = BaseMoveSpeed;

	if (Character->GetTpsCamera())
	{
		DefaultFOV = Character->GetTpsCamera()->FieldOfView;
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
		//InterpFov(DeltaTime);
		InterpTransition(DeltaTime);
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
	DOREPLIFETIME(UCombatComponent, GrenadeCur);	
}
void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Unoccupied:
		if (bIsAttackDown)
		{
			Attack();
		}
		break;
	case ECombatState::ECS_Reloading:
		WeaponAmmoInsertion();
		break;
	case ECombatState::ECS_Throwing:
		if (Character && !Character->IsLocallyControlled())
		{
			ShowAttachedGrenade(true);
			Character->PlayThrowMontage();
			AttachActorToHand(EquippedWeapon, FName("LeftHandSocket"));
		}
		break;
	}
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

void UCombatComponent::InterpFov(float _deltaTime)
{
	if (!EquippedWeapon) return;

	if (bIsADS)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetAdsFov(), _deltaTime, EquippedWeapon->GetAdsInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, _deltaTime, ZoomInterpSpeed);
	}

	if (Character && Character->GetTpsCamera())
	{
		Character->GetTpsCamera()->SetFieldOfView(CurrentFOV);
	}
}
void UCombatComponent::InterpTransition(float _deltaTime)
{
	// interp 할지 말지 정해야함
	if (!Character->IsInterpTransition()) return;

	// 타이머 진행
	TransitionTimeCur += _deltaTime;

	float alpha = FMath::Clamp(TransitionTimeCur / TransitionTimeMax, 0.f, 1.f);

	// 정보 가져오기
	UCameraComponent* fpsCam = Character->GetFpsCamera();
	UCameraComponent* tpsCam = Character->GetTpsCamera();
	FTransform fpsTransform = fpsCam->GetComponentTransform();
	FTransform tpsTransform = tpsCam->GetComponentTransform();
	UCapsuleComponent* parentComponent = Character->GetCapsuleComponent();

	// 완료 조건
	if (TransitionTimeCur >= TransitionTimeMax - (TransitionTimeMax / 1.5f))
	{
		TransitionTimeCur = 0.f;
		Character->SetInterpTransition(false);

		if (bIsADS)
		{
			tpsCam->SetRelativeTransform(Character->GetTpsRelativeTransform());
			Character->GetFpsCamera()->SetActive(true);
			Character->GetTpsCamera()->SetActive(false);
			Character->bUseControllerRotationYaw = true;
		}
		else
		{
			fpsCam->SetRelativeTransform(Character->GetFpsRelativeTransform());
			Character->GetFpsCamera()->SetActive(false);
			Character->GetTpsCamera()->SetActive(true);
			Character->bUseControllerRotationYaw = false;
		}
		return;
	}

	// 보간 적용
	FVector interpLocation;
	FQuat interpRotation;
	if (bIsADS)
	{
		interpLocation = FMath::Lerp(tpsTransform.GetLocation(), fpsTransform.GetLocation(), alpha);
		interpRotation = FQuat::Slerp(tpsTransform.GetRotation(), fpsTransform.GetRotation(), alpha);
		tpsCam->SetWorldLocation(interpLocation);

		// FQuat을 FRotator로 변환 
		FRotator interpRotator = interpRotation.Rotator(); // pitch 값을 0으로 설정 
		interpRotator.Pitch = 0.0f; // 수정된 FRotator를 다시 FQuat으로 변환 
		interpRotation = interpRotator.Quaternion();
		Character->SetActorRotation(interpRotation);
		//parentComponent->SetWorldRotation(interpRotation);
	}
	else
	{
		interpLocation = FMath::Lerp(fpsTransform.GetLocation(), tpsTransform.GetLocation(), alpha);
		interpRotation = FQuat::Slerp(fpsTransform.GetRotation(), tpsTransform.GetRotation(), alpha);
		fpsCam->SetWorldLocation(interpLocation);
		fpsCam->SetWorldRotation(interpRotation);
	}

}
void UCombatComponent::SetADS(bool _bIsADS)
{
	if (nullptr == Character || nullptr == EquippedWeapon) return;

	bIsADS = _bIsADS;
	ServerSetADS(_bIsADS);
	Character->GetCharacterMovement()->MaxWalkSpeed = bIsADS ? ADSMoveSpeed : BaseMoveSpeed;

	// 트렌지션 안햇으면
	if (!Character->IsInterpTransition())
	{
		Character->SetInterpTransition(true);
	}

	// 스코프 보이기 | 말기
	if (Character->IsLocallyControlled() && EquippedWeapon->IsUsingScope())
	{
		Character->ShowSniperScopeWidget(bIsADS);
	}
}
void UCombatComponent::ServerSetADS_Implementation(bool _bIsADS)
{
	bIsADS = _bIsADS;
	if (!Character) return;
	Character->GetCharacterMovement()->MaxWalkSpeed = bIsADS ? ADSMoveSpeed : BaseMoveSpeed;

	if (!Character->IsInterpTransition())
	{
		Character->SetInterpTransition(true);
	}

}




void UCombatComponent::EquipWeapon(ABaseWeapon* _weaponToEquip)
{
	if (nullptr == Character || nullptr == _weaponToEquip) return;
	if (ECombatState::ECS_Unoccupied != CombatState) return;

	DropEquippedWeapon();

	EquippedWeapon = _weaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	AttachActorToHand(EquippedWeapon, FName("RightHandSocket"));

	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetHUDCurrentAmmo();

	UpdateExtraAmmo();
	PlayEquipWeaponSound();
	WeaponAutoReload();

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}
void UCombatComponent::OnRep_EquipWeapon()
{
	if (EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		
		AttachActorToHand(EquippedWeapon, FName("RightHandSocket"));
		PlayEquipWeaponSound();

		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}	
}
void UCombatComponent::DropEquippedWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}
}
void UCombatComponent::PlayEquipWeaponSound()
{
	if (Character && EquippedWeapon && EquippedWeapon->GetEquipSound())
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquippedWeapon->GetEquipSound(), Character->GetActorLocation());
	}
}



void UCombatComponent::AttachActorToHand(AActor* _actorToAttach, FName _socket)
{
	if (!Character || !Character->GetMesh() || !_actorToAttach) return;

	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(_socket);
	if (HandSocket)
	{
		HandSocket->AttachActor(_actorToAttach, Character->GetMesh());
	}

}


/* 
* call by Character
*/
void UCombatComponent::InputAttack(bool _presseed)
{
	bIsAttackDown = _presseed;

	if (!bIsAttackDown) return;

	Attack();

}

void UCombatComponent::Attack()
{
	if (!EquippedWeapon) return;
	if (CanFire())
	{
		bCanAttack = false;
		ServerAttack(HitTarget);
		CrosshairAttackingFactor = SpreadMOA;
		StartFireTimer();
	}
	else
	{
		WeaponAutoReload();
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
	if (ECombatState::ECS_Reloading == CombatState && EWeaponType::EWT_ScatterGun == EquippedWeapon->GetWeaponType())
	{
		Character->PlayFireMontage(bIsADS);
		EquippedWeapon->FireRound(_traceHitTarget);
		CombatState = ECombatState::ECS_Unoccupied;
		return;
	}
	if (ECombatState::ECS_Unoccupied == CombatState)
	{
		Character->PlayFireMontage(bIsADS);
		EquippedWeapon->FireRound(_traceHitTarget);
	}
}

void UCombatComponent::StartFireTimer()
{
	if (!EquippedWeapon || !Character) return;

	Character->GetWorldTimerManager().SetTimer(FireTimer, this, &UCombatComponent::FireTimerFinished, EquippedWeapon->GetFireRate());
}
void UCombatComponent::FireTimerFinished()
{
	bCanAttack = true;
	if (bIsAttackDown && EquippedWeapon->IsUsingAutoFire())
	{
		Attack();
	}
}





void UCombatComponent::UpdateExtraAmmo()
{
	if (!EquippedWeapon) return;
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
}

void UCombatComponent::WeaponAutoReload()
{
	if (!EquippedWeapon) return;


	// 약실 장전 조건
	if (!EquippedWeapon->IsAmmoEmpty())
	{
		if (EquippedWeapon->IsChamberEmpty())
		{
			WeaponChamberingRound();
		}
	}
	// 탄창 교체 조건
	else if (0 < ExtraAmmo)
	{
		WeaponAmmoInsertion();
	}
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
	ExtraAmmoMap.Emplace(EWeaponType::EWT_SubmachineGun, StartingAmmoSMG);
	ExtraAmmoMap.Emplace(EWeaponType::EWT_ScatterGun, StartingAmmoScatterGun);
	ExtraAmmoMap.Emplace(EWeaponType::EWT_MarksmanRifle, StartingAmmoDMR);
	ExtraAmmoMap.Emplace(EWeaponType::EWT_GrenadeLauncher, StartingAmmoGrenade);

}
void UCombatComponent::UpdateMagazineAmmo()
{
	if (!Character || !EquippedWeapon) return;

	/*int32 reloadAmount = AmountToReload();
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
	EquippedWeapon->AddAmmo(reloadAmount);*/

	// 여분 탄 까고
	if (ExtraAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		ExtraAmmoMap[EquippedWeapon->GetWeaponType()] -= 1;
		ExtraAmmo = ExtraAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	// 화면에 출력
	PlayerController = nullptr == PlayerController ? Cast<ABountyPlayerController>(Character->Controller) : PlayerController;
	if (PlayerController)
	{
		PlayerController->SetHUD_ExtraAmmo(ExtraAmmo);
	}
	
	// 재장전
	if (EquippedWeapon->IsUsingMagazine())
	{
		EquippedWeapon->AddAmmo(EquippedWeapon->GetMagCapacity());
	}
	else
	{
		EquippedWeapon->AddAmmo(1);
	}
	bCanAttack = true;
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

void UCombatComponent::WeaponAmmoInsertion()
{
	if (0 >= ExtraAmmo || ECombatState::ECS_Unoccupied != CombatState) return;

	if (EquippedWeapon->IsUsingMagazine())
	{
		ServerAmmoInsertion();
	}
	else if (!EquippedWeapon->IsAmmoFull())
	{
		ServerAmmoInsertion();
	}	
}
void UCombatComponent::WeaponChamberingRound()
{
	if (ECombatState::ECS_Unoccupied == CombatState)
	{
		ServerChamberingRound();
	}
}
void UCombatComponent::ServerAmmoInsertion_Implementation()
{
	if (!Character || !EquippedWeapon) return;

	CombatState = ECombatState::ECS_Reloading;
	Character->PlayAmmoInsertion();
}
void UCombatComponent::ServerChamberingRound_Implementation()
{
	if (!Character || !EquippedWeapon) return;

	CombatState = ECombatState::ECS_Reloading;
	Character->PlayChamberingRound();
}

void UCombatComponent::AmmoInsertion()
{
	if (!Character) return;
	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateMagazineAmmo();
	}
	if (bIsAttackDown)
	{
		Attack();
	}
	UE_LOG(LogTemp, Warning, TEXT("Ammo Cur : %d"), EquippedWeapon->GetAmmo());
	if (EquippedWeapon->IsChamberEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("chamber Empty"));
	}
}

void UCombatComponent::ChamberingRound()
{
	if (!Character) return;
	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;		
		EquippedWeapon->ChamberingRound();
	}
	if (bIsAttackDown)
	{
		Attack();
	}
}

bool UCombatComponent::CanFire()
{
	if (!EquippedWeapon) return false;


	const bool bChamberNotEmpty = !EquippedWeapon->IsChamberEmpty();
	const bool bWeaponReady = bCanAttack && bChamberNotEmpty;

	if (!EquippedWeapon->IsUsingMagazine())
	{
		return bWeaponReady;
	}
	return bWeaponReady && (CombatState == ECombatState::ECS_Unoccupied);
}




void UCombatComponent::ThrowGrenade()
{
	if (0 == GrenadeCur) return;
	if (ECombatState::ECS_Unoccupied != CombatState) return;
	if (!EquippedWeapon) return;
	CombatState = ECombatState::ECS_Throwing;
	if (Character)
	{
		ShowAttachedGrenade(true);
		Character->PlayThrowMontage();
		FName socketName = "LeftHandSocket";
		if (EWeaponType::EWT_Pistol == EquippedWeapon->GetWeaponType() || EWeaponType::EWT_SubmachineGun == EquippedWeapon->GetWeaponType())
		{
			socketName = "LeftHandShortSocket";
		}
		AttachActorToHand(EquippedWeapon, socketName);
	}
	if (Character && !Character->HasAuthority())
	{
		ServerThrowGrenade();
	}
	if (Character && Character->HasAuthority())
	{
		GrenadeCur = FMath::Clamp(GrenadeCur - 1, 0, GrenadeMax);
		UpdateHUDGrenades();
	}
}
void UCombatComponent::ServerThrowGrenade_Implementation()
{
	if (0 == GrenadeCur) return;
	CombatState = ECombatState::ECS_Throwing;
	if (Character)
	{
		ShowAttachedGrenade(true);
		Character->PlayThrowMontage();
		FName socketName = "LeftHandSocket";
		if (EWeaponType::EWT_Pistol == EquippedWeapon->GetWeaponType() || EWeaponType::EWT_SubmachineGun == EquippedWeapon->GetWeaponType())
		{
			socketName = "LeftHandShortSocket";
		}
		AttachActorToHand(EquippedWeapon, socketName);
	}
	GrenadeCur = FMath::Clamp(GrenadeCur - 1, 0, GrenadeMax);
	UpdateHUDGrenades();
}
void UCombatComponent::ThrowGrenadeFinished()
{
	CombatState = ECombatState::ECS_Unoccupied;
	AttachActorToHand(EquippedWeapon, FName("RightHandSocket"));
}

void UCombatComponent::LaunchGrenade()
{
	ShowAttachedGrenade(false);
	if (Character && Character->IsLocallyControlled())
	{
		ServerLaunchGrenade(HitTarget);
	}
}

void UCombatComponent::ServerLaunchGrenade_Implementation(const FVector_NetQuantize& _target)
{
	if (Character && GrenadeClass && Character->GetAttachedGrenade())
	{
		const FVector startingLocation = Character->GetAttachedGrenade()->GetComponentLocation();
		FVector toTarget = _target - startingLocation;
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = Character;
		spawnParams.Instigator = Character;
		UWorld* world = GetWorld();
		if (world)
		{
			AProjectile* grenade = world->SpawnActor<AProjectile>(GrenadeClass, startingLocation, toTarget.Rotation(), spawnParams);
		}
	}
}

void UCombatComponent::ShowAttachedGrenade(bool _show)
{
	if (Character && Character->GetAttachedGrenade())
	{
		Character->GetAttachedGrenade()->SetVisibility(_show);
	}
}


void UCombatComponent::OnRep_Grenades()
{
	UpdateHUDGrenades();
}

void UCombatComponent::UpdateHUDGrenades()
{
	PlayerController = PlayerController == nullptr ? Cast<ABountyPlayerController>(Character->Controller) : PlayerController;
	if (PlayerController)
	{
		PlayerController->SetHUD_GrenadeCount(GrenadeCur);
	}
}
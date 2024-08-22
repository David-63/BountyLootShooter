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
#include "Camera/CameraComponent.h"

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

void UCombatComponent::TraceUnderCrosshairs(FHitResult& _traceHitResult)
{
	// 뷰포트 가져옴
	FVector2D viewportSize;	
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(viewportSize);
	}
	
	// 화면 중앙 좌표
	FVector2D crossHairLocation(viewportSize.X / 2.f, viewportSize.Y / 2.f);

	// 역행렬 계산으로 담을 조준점의 위치와 방향
	FVector crossHairWorldPosition, crossHairWorldDirection;

	// InertiaValue 값에 5.f 만큼 곱해서 관성의 크기를 키움
	FVector2D inertiaValue = Character->GetInertiaValue() * InertiaMagnitude;
	crossHairLocation.X += inertiaValue.X;
	crossHairLocation.Y += inertiaValue.Y;

	// 관성이 적용된 조준점 위치로부터 역행렬 계산으로 screen -> World 변환
	bool isScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
		(UGameplayStatics::GetPlayerController(this, 0), crossHairLocation, crossHairWorldPosition, crossHairWorldDirection);

	if (!isScreenToWorld) return;

	FVector begin = crossHairWorldPosition;
	FVector end = begin + crossHairWorldDirection * TRACE_LENGTH;
	

	// Ray 결과 반환
	GetWorld()->LineTraceSingleByChannel(_traceHitResult, begin, end, ECollisionChannel::ECC_Visibility);

	if (!_traceHitResult.bBlockingHit)
	{
		_traceHitResult.ImpactPoint = end;
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
	
	// calculate crosshair spread

	FVector2D moveSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
	if (Character->bIsCrouched)
	{
		moveSpeedRange.Y = Character->GetCharacterMovement()->MaxWalkSpeedCrouched;
	}
	if (Character->GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.5f, _deltaTime, 2.5f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, _deltaTime, 25.f);
	}

	FVector2D VelocityRatioRange(0.f, 1.f);
	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.f;
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(moveSpeedRange, VelocityRatioRange, Velocity.Size());
	hudPackage.SpreadFactor = CrosshairVelocityFactor + CrosshairInAirFactor;


	// 여기에서 Vector2로 Crosshair Inertia value 전달하면 될듯?
	HUD->SetHUDPackage(hudPackage, Character->GetInertiaValue() * InertiaMagnitude);

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
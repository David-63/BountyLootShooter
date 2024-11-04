// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Components/WidgetComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/InputDeviceSubsystem.h"

#include "Net/UnrealNetwork.h"
#include "Bounty/Weapon/BaseWeapon.h"
#include "Bounty/BountyComponents/CombatComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Bounty/Bounty.h"

#include "Bounty/PlayerController/BountyPlayerController.h"
#include "Bounty/GameMode/BountyGameMode.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Bounty/PlayerState/BountyPlayerState.h"
#include "Bounty/Weapon/WeaponTypes.h"


ABountyCharacter::ABountyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GetCapsuleComponent()->InitCapsuleSize(34.f, 88.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 800.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->JumpZVelocity = 800.f;
	GetCharacterMovement()->AirControl = 0.35f;

	GetCharacterMovement()->MaxWalkSpeed = 550.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	GetCharacterMovement()->MaxWalkSpeedCrouched = 250.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 120.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Camera setup
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);

	// ui
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);
		
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;


	// Elim Effect
	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

}
void ABountyCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUD_Health();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ABountyCharacter::ReceiveDamage);
	}
}

void ABountyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateInPlace(DeltaTime);

	HideCharacterMesh();
	PollInit();
}
void ABountyCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (ENetRole::ROLE_SimulatedProxy < GetLocalRole() && IsLocallyControlled())
	{
		ADS_Offset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (0.25f < TimeSinceLastMovementReplication)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}
void ABountyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_Character, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhancedInputComponent->BindAction(IA_JumpNDodge, ETriggerEvent::Started, this, &ABountyCharacter::Jump);
		EnhancedInputComponent->BindAction(IA_JumpNDodge, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ABountyCharacter::InputMove);
		EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ABountyCharacter::InputLook);
		EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Started, this, &ABountyCharacter::InputCrouch);

		EnhancedInputComponent->BindAction(IA_Equip, ETriggerEvent::Triggered, this, &ABountyCharacter::InputEquip);
		EnhancedInputComponent->BindAction(IA_ADS, ETriggerEvent::Started, this, &ABountyCharacter::InputADS);
		EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Started, this, &ABountyCharacter::InputFireDown);
		EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Completed, this, &ABountyCharacter::InputFireRelease);
		EnhancedInputComponent->BindAction(IA_ReloadNSwap, ETriggerEvent::Triggered, this, &ABountyCharacter::InputReload);
		EnhancedInputComponent->BindAction(IA_Throw, ETriggerEvent::Triggered, this, &ABountyCharacter::InputThrow);

	}
}
void ABountyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
}


// rep function
void ABountyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// DOREPLIFETIME(ABountyCharacter, OverlappingWeapon); 변경사항이 생기면 리플리케이트한다

	DOREPLIFETIME_CONDITION(ABountyCharacter, OverlappingWeapon, COND_OwnerOnly); // 변동사항이 생기고, 조건에 해당하는 경우 리플리케이트한다
	DOREPLIFETIME(ABountyCharacter, Health_Cur);
	
}

void ABountyCharacter::Destroyed()
{
	Super::Destroyed();
	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}

	ABountyGameMode* bountyGameMode = Cast<ABountyGameMode>(UGameplayStatics::GetGameMode(this));
	bool bmatchNotInProgress = bountyGameMode && bountyGameMode->GetMatchState() != MatchState::InProgress;
	if (Combat && Combat->EquippedWeapon && bmatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}
}

void ABountyCharacter::ServerInputEquip_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}
void ABountyCharacter::SetOverlappingWeapon(ABaseWeapon* _weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = _weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}




void ABountyCharacter::OnRep_OverlappingWeapon(ABaseWeapon* _lastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (_lastWeapon)
	{
		_lastWeapon->ShowPickupWidget(false);
	}
}
void ABountyCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;

}
void ABountyCharacter::OnRep_Health()
{
	UpdateHUD_Health();
	PlayHitReactMontage();
}





// update function
void ABountyCharacter::ADS_Offset(float _deltaTime)
{
	if (Combat && nullptr == Combat->EquippedWeapon) return;
	float moveSpeed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();
	if (0.f == moveSpeed && !bIsInAir) // standing still, not jumping
	{
		bIsRotateRootBone = true;

		FRotator currentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator deltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(currentAimRotation, StartingAimRotation);
		AO_Yaw = deltaRotation.Yaw;
		if (ETurningInPlace::ETIP_NotTurning == TurningInPlace)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(_deltaTime);

	}
	if (0.f < moveSpeed || bIsInAir) // running, or jumping
	{
		bIsRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();

}
void ABountyCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (90.f < AO_Pitch && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		FVector2D inRange(270.f, 360.f);
		FVector2D outRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(inRange, outRange, AO_Pitch);
	}
}
void ABountyCharacter::TurnInPlace(float _deltaTime)
{
	if (90.f < AO_Yaw)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (-90.f > AO_Yaw)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (ETurningInPlace::ETIP_NotTurning != TurningInPlace)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, _deltaTime, 10.f);
		AO_Yaw = InterpAO_Yaw;
		if (15.f > FMath::Abs(AO_Yaw))
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}
void ABountyCharacter::SimProxiesTurn()
{
	if (!Combat || !Combat->EquippedWeapon) return;

	bIsRotateRootBone = false; 
	float moveSpeed = CalculateSpeed();
	if (0.f < moveSpeed)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	
	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

}
void ABountyCharacter::HideCharacterMesh()
{
	if (!IsLocallyControlled()) return;

	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		//if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		//{
		//	Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		//}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		//if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		//{
		//	Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		//}
	}
}
float ABountyCharacter::CalculateSpeed() const
{
	FVector velocity = GetVelocity();
	velocity.Z = 0.f;
	return velocity.Size();
}




// Player stats
void ABountyCharacter::PlayHitReactMontage()
{
	if (nullptr == Combat || nullptr == Combat->EquippedWeapon) return;


	if (HitReactMontage)
	{
		FName sessionName("FromFront");
		Super::PlayAnimMontage(HitReactMontage, 1.f, sessionName);
	}
}
void ABountyCharacter::PlayThrowMontage()
{
	if (ThrowMontage)
	{
		//FName sessionName("FromFront");
		Super::PlayAnimMontage(ThrowMontage, 1.4f);
	}	
}


void ABountyCharacter::PollInit()
{
	if (nullptr == BountyPlayerState)
	{
		BountyPlayerState = GetPlayerState<ABountyPlayerState>();
		if (BountyPlayerState)
		{
			BountyPlayerState->AddToScore(0.f);
			BountyPlayerState->AddToLifeLoss(0);
		}
	}

}

void ABountyCharacter::UpdateHUD_Health()
{
	BountyPlayerController = nullptr == BountyPlayerController ? Cast<ABountyPlayerController>(Controller) : BountyPlayerController;
	if (BountyPlayerController)
	{
		BountyPlayerController->SetHUD_Health(Health_Cur, Health_Max);
	}
}

void ABountyCharacter::ReceiveDamage(AActor* _damagedActor, float _damage, const UDamageType* _damageType, AController* _instegatorController, AActor* _damageCauser)
{
	Health_Cur = FMath::Clamp(Health_Cur - _damage, 0.f, Health_Max);
	UpdateHUD_Health();
	PlayHitReactMontage();

	if (0.f >= Health_Cur)
	{
		ABountyGameMode* bountyGamemode = GetWorld()->GetAuthGameMode<ABountyGameMode>();
		if (bountyGamemode)
		{
			BountyPlayerController = nullptr == BountyPlayerController ? Cast<ABountyPlayerController>(Controller) : BountyPlayerController;
			ABountyPlayerController* attackerController = Cast<ABountyPlayerController>(_instegatorController);
			bountyGamemode->PlayerEliminated(this, BountyPlayerController, attackerController);
		}
	}
	
}



void ABountyCharacter::PlayFireMontage(bool _bADS)
{
	if (nullptr == Combat || nullptr == Combat->EquippedWeapon) return;


	if (FireArmMontage)
	{
		FName sessionName;
		sessionName = _bADS ? FName("RifleADS") : FName("RifleHip");
		Super::PlayAnimMontage(FireArmMontage, 1.f, sessionName);
	}
}

void ABountyCharacter::PlayReloadMontage()
{
	if (nullptr == Combat || nullptr == Combat->EquippedWeapon) return;


	if (ReloadMontage)
	{
		FName sessionName;
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			sessionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			sessionName = FName("Rifle");
			break;
		case EWeaponType::EWT_Pistol:
			sessionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubmachineGun:
			sessionName = FName("Rifle");
			break;
		case EWeaponType::EWT_ScatterGun:
			sessionName = FName("ShotGun");
			break;
		case EWeaponType::EWT_MarksmanRifle:
			sessionName = FName("SniperRifle");
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			sessionName = FName("Rifle");
			break;
		}

		Super::PlayAnimMontage(ReloadMontage, 1.4f, sessionName);
	}
}


// Elim function
void ABountyCharacter::Elim()
{
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->Dropped();
	}

	MulticastElim();
	GetWorldTimerManager().SetTimer(ElimTimer, this, &ABountyCharacter::ElimTimerFinished, ElimDelay);
}
void ABountyCharacter::MulticastElim_Implementation()
{
	if (BountyPlayerController)
	{
		BountyPlayerController->SetHUD_CurrentAmmo(0);
	}
	bIsElimmed = true;
	PlayElimMontage();

	// Start dissolve effect
	if (DissolveMaterialInstanceA && DissolveMaterialInstanceB)
	{
		DynamicDissolveMaterialInstanceA = UMaterialInstanceDynamic::Create(DissolveMaterialInstanceA, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstanceA);
		DynamicDissolveMaterialInstanceB = UMaterialInstanceDynamic::Create(DissolveMaterialInstanceB, this);
		GetMesh()->SetMaterial(1, DynamicDissolveMaterialInstanceB);
		DynamicDissolveMaterialInstanceA->SetScalarParameterValue(TEXT("Dissolve"), 0.6f);
		DynamicDissolveMaterialInstanceA->SetScalarParameterValue(TEXT("Glow"), 300.f);
		DynamicDissolveMaterialInstanceB->SetScalarParameterValue(TEXT("Dissolve"), 0.6f);
		DynamicDissolveMaterialInstanceB->SetScalarParameterValue(TEXT("Glow"), 300.f);
	}
	StartDissolve();
	bDisableGameplay = true;

	if (Combat)
	{
		Combat->InputAttack(false);
	}

	// Disable character
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	if (BountyPlayerController)
	{
		DisableInput(BountyPlayerController);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Spawn elim bot
	if (ElimBotEffect)
	{
		FVector elimBotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		ElimBotComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ElimBotEffect, elimBotSpawnPoint, GetActorRotation());
	}
	if (ElimBotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, ElimBotSound, GetActorLocation());
	}

	bool releaseScope = 
		IsLocallyControlled() && Combat && Combat->bIsADS && Combat->EquippedWeapon && 
		EWeaponType::EWT_MarksmanRifle == Combat->EquippedWeapon->GetWeaponType();

	if (releaseScope)
	{
		ShowSniperScopeWidget(false);
	}
}
void ABountyCharacter::ElimTimerFinished()
{
	ABountyGameMode* bountyGamemode = GetWorld()->GetAuthGameMode<ABountyGameMode>();
	if (bountyGamemode)
	{		
		bountyGamemode->RequestRespawn(this, Controller);
	}
	
}

void ABountyCharacter::UpdateDissolveMaterial(float _dissolveValue)
{
	if (!DynamicDissolveMaterialInstanceA || !DynamicDissolveMaterialInstanceB || !DissolveTimeline) return;

	DynamicDissolveMaterialInstanceA->SetScalarParameterValue(TEXT("Dissolve"), _dissolveValue);
	DynamicDissolveMaterialInstanceB->SetScalarParameterValue(TEXT("Dissolve"), _dissolveValue);

}
void ABountyCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ABountyCharacter::UpdateDissolveMaterial);
	if (DissolveCurve)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}
void ABountyCharacter::PlayElimMontage()
{
	if (ElimMontage)
	{
		Super::PlayAnimMontage(ElimMontage);
	}
}


ABaseWeapon* ABountyCharacter::GetEquippedWeapon() const
{
	if (nullptr == Combat) return nullptr;
	return Combat->EquippedWeapon;
}
FVector ABountyCharacter::GetHitTarget() const
{
	if (!Combat) return FVector();

	return Combat->HitTarget;
}
bool ABountyCharacter::IsUsingGamepad() const
{
	UInputDeviceSubsystem* inputDeviceSubsystem = UInputDeviceSubsystem::Get();
	if (inputDeviceSubsystem)
	{
		FHardwareDeviceIdentifier device = inputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(GetPlatformUserId());
		if (device.PrimaryDeviceType == EHardwareDevicePrimaryType::Gamepad)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}
bool ABountyCharacter::IsWeaponEquipped() const
{
	return (Combat && Combat->EquippedWeapon);
}
bool ABountyCharacter::IsADS() const
{
	return (Combat && Combat->bIsADS);
}
ECombatState ABountyCharacter::GetCombatState() const
{
	if (!Combat) return ECombatState::ECS_MAX;

	return Combat->CombatState;
}


// controll function
void ABountyCharacter::InputMove(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		if (IsUsingGamepad())
		{
			float magnitudeY = MovementVector.Y / 50.f;
			float magnitudeX = MovementVector.X / 50.f;

			AddMovementInput(ForwardDirection, magnitudeY);
			AddMovementInput(RightDirection, magnitudeX);
		}
		else
		{
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}
	}
}
void ABountyCharacter::InputLook(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	InertiaValue = LookAxisVector * 0.25f;
	
	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
void ABountyCharacter::InputEquip(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;

	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerInputEquip();
		}
	}
}
void ABountyCharacter::InputCrouch()
{
	if (bDisableGameplay) return;
	if (!IsWeaponEquipped()) return;
	if (bIsCrouched)
	{
		ACharacter::UnCrouch();
	}
	else
	{
		ACharacter::Crouch();
	}
}
void ABountyCharacter::InputADS()
{
	if (!Combat) return;

	Combat->bIsADS ? Combat->SetADS(false) : Combat->SetADS(true);

}
void ABountyCharacter::InputFireDown(const FInputActionValue& Value)
{
	if (!Combat) return;
	if (bDisableGameplay) return;
	Combat->InputAttack(true);
}
void ABountyCharacter::InputFireRelease(const FInputActionValue& Value)
{
	if (!Combat) return;
	
	Combat->InputAttack(false);
}
void ABountyCharacter::Jump()
{
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void ABountyCharacter::InputReload()
{
	if (Combat)
	{
		Combat->WeaponReload();
	}
}

void ABountyCharacter::InputThrow()
{
	if (Combat)
	{
		Combat->ThrowGrenade();
	}
}

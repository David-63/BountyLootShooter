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



ABountyCharacter::ABountyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

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
	CameraBoom->TargetArmLength = 70.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Camera setup
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// ui
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);
		
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

}

void ABountyCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ABountyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ADS_Offset(DeltaTime);
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
		EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Started, this, &ABountyCharacter::InputFire);
		
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

void ABountyCharacter::ADS_Offset(float _deltaTime)
{
	if (Combat && nullptr == Combat->EquippedWeapon) return;
	FVector velocity = GetVelocity();
	velocity.Z = 0.f;
	float moveSpeed = velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();
	if (0.f == moveSpeed && !bIsInAir) // standing still, not jumping
	{
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
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

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

void ABountyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// DOREPLIFETIME(ABountyCharacter, OverlappingWeapon); 변경사항이 생기면 리플리케이트한다

	DOREPLIFETIME_CONDITION(ABountyCharacter, OverlappingWeapon, COND_OwnerOnly); // 변동사항이 생기고, 조건에 해당하는 경우 리플리케이트한다

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

ABaseWeapon* ABountyCharacter::GetEquippedWeapon() const
{
	if (nullptr == Combat) return nullptr;
	return Combat->EquippedWeapon;
}

void ABountyCharacter::PlayFireArmMontage(bool _bADS)
{
	if (nullptr == Combat || nullptr == Combat->EquippedWeapon) return;


	if (FireArmMontage)
	{
		GEngine->AddOnScreenDebugMessage(3, 0.1f, FColor::Blue, FString::Printf(TEXT("PlayMontage")));
		FName sessionName;
		sessionName = _bADS ? FName("RifleADS") : FName("RifleHip");
		Super::PlayAnimMontage(FireArmMontage, 1.f, sessionName);
	}
}

FVector ABountyCharacter::GetHitTarget() const
{
	if (!Combat) return FVector();

	return Combat->HitTarget;
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

void ABountyCharacter::ServerInputEquip_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}



void ABountyCharacter::InputMove(const FInputActionValue& Value)
{
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

void ABountyCharacter::InputFire()
{
	if (!Combat) return;

	Combat->Attack(true);
}

void ABountyCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

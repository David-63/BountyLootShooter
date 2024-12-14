// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "BountyShooter/D_LogCategory.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

#include "Kismet/GameplayStatics.h"

#include "ShooterMovementHandler.h"
#include "ShooterCombatHandler.h"
#include "ShooterInventoryHandler.h"

#include "BountyShooter/Items/ItemBase.h"

AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SetNetUpdateFrequency(66.f);
	SetMinNetUpdateFrequency(33.f);
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);
	SpringArm3P = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm 3P"));
	SpringArm3P->SetupAttachment(RootComponent);
	SpringArm3P->TargetArmLength = 400.0f;
	SpringArm3P->bUsePawnControlRotation = true;

	Camera3P = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera 3P"));
	Camera3P->SetupAttachment(SpringArm3P, USpringArmComponent::SocketName);
	Camera3P->bUsePawnControlRotation = false;
	
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FHitResult result;
	LineTraceViewDirection(result);
}


void AShooterCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Look);
		EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Interaction);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AShooterCharacter::LineTraceViewDirection(FHitResult& result)
{
	// center location
	FVector2D viewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(viewportSize);
	}
	FVector2D crossHairLocation(viewportSize.X / 2.f, viewportSize.Y / 2.f);

	// Calculate inverse matrix from the crosshair position with inertia applied | screen -> World
	FVector crossHairWorldPosition, crossHairWorldDirection;
	bool isScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
	(UGameplayStatics::GetPlayerController(this, 0), crossHairLocation, crossHairWorldPosition, crossHairWorldDirection);

	if (!isScreenToWorld) return;

	FVector begin = crossHairWorldPosition;
	float distanceToCharacter = (GetActorLocation() - begin).Size();
	begin += crossHairWorldDirection * (distanceToCharacter + 100.f);
	FVector end = begin + crossHairWorldDirection * TRACE_LENGTH;


	// Ray result return
	GetWorld()->LineTraceSingleByChannel(result, begin, end, ECollisionChannel::ECC_Visibility);

	if (!result.bBlockingHit)
	{
		result.ImpactPoint = end;
	}
	HitLocation = end;
	HitTarget = result.GetActor();
}

void AShooterCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AShooterCharacter::Interaction()
{
	// 다양한 조건이 있겠지만 당장은 아이템 기능 구현
	if (!InventoryHandler)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryHandler not binded"));
		return;
	}

	if (!OverlappingItems.IsEmpty())
	{
		AItemBase* targetItem = Cast<AItemBase>(HitTarget);
		if (OverlappingItems.Contains(targetItem))
		{
			OverlappingItems.Remove(targetItem);

			if (!InventoryHandler->Primary)
			{
				InventoryHandler->Primary = targetItem;
				targetItem->PickupDisable();
				UE_LOG(LogTemp, Warning, TEXT("Inventory primary added: %s"), *InventoryHandler->Primary->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Already have item"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Wrong Direction"));
		}
	}
}

bool AShooterCharacter::IsUsingGamepad() const
{
	UInputDeviceSubsystem* inputDeviceSubsystem = UInputDeviceSubsystem::Get();
	if (inputDeviceSubsystem)
	{
		FHardwareDeviceIdentifier device = inputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(GetPlatformUserId());
		return device.PrimaryDeviceType == EHardwareDevicePrimaryType::Gamepad;
	}
	return false;
}

void AShooterCharacter::SetOverlappingItems(AItemBase* Item, bool bShouldAdd)
{
	if (bShouldAdd)
	{
		if (!OverlappingItems.Contains(Item))
		{
			OverlappingItems.Add(Item);
			UE_LOG(LogTemp, Warning, TEXT("Item added: %s"), *Item->GetName());
		}
	}
	else
	{
		if (OverlappingItems.Contains(Item))
		{
			OverlappingItems.Remove(Item);
			UE_LOG(LogTemp, Warning, TEXT("Item removed: %s"), *Item->GetName());
		}
	}
}



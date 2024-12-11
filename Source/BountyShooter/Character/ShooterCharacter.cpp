// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "BountyShooter/D_LogCategory.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "CharacterTrajectoryComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "ShooterMovementHandler.h"


AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SetNetUpdateFrequency(66.f);
	SetMinNetUpdateFrequency(33.f);

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.25f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	// Create a camera boom (pulls in towards the player if there is a collision)
	SpringArm3P = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm 3P"));
	SpringArm3P->SetupAttachment(RootComponent);
	SpringArm3P->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	SpringArm3P->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	Camera3P = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera 3P"));
	Camera3P->SetupAttachment(SpringArm3P, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	Camera3P->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	

	//MovementHandler = CreateDefaultSubobject<UShooterMovementHandler>(TEXT("MovementHander"));
	//MovementHandler->MappingMovementContext(this); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation

	//SpringArm1P = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm 1P"));
	//SpringArm1P->SetupAttachment(GetMesh(), FName(TEXT("head")));
	//SpringArm1P->TargetArmLength = 0.0f; // The camera follows at this distance behind the character	
	//SpringArm1P->SetRelativeLocation(FVector(2.f, 5.f, 0.f));
	//SpringArm1P->SetRelativeRotation(FRotator(0.f, 90.f, -90.f).Quaternion()); //FVector(-90.f, 0.f, 90.f)
	//SpringArm1P->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	//Camera1P = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera1P"));
	//Camera1P->SetupAttachment(SpringArm1P); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	//Camera1P->bAutoActivate = false;

}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
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



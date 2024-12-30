// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterMovementHandler.h"
#include "ShooterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BountyShooter/BountyComponents/ShooterEnums.h"
#include "ShooterAnimInstance.h"

// Sets default values for this component's properties
UShooterMovementHandler::UShooterMovementHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UShooterMovementHandler::BeginPlay()
{
	Super::BeginPlay();

	FGateSetting walkSetting;
	walkSetting.MaxMoveSpeed = 200.f;
	walkSetting.MaxAcceleration = 400.f;
	walkSetting.BrakingDeceleration = 400.f;
	walkSetting.BrakingFrictionFactor = 1.0f;
	walkSetting.BrakingFriction = 1.0f;
	walkSetting.UseSeperateBrakingFriction = true;
	GateSetting.Add(EGate::EG_Walk, walkSetting);

	FGateSetting jogSetting;
	jogSetting.MaxMoveSpeed = 450.f;
	jogSetting.MaxAcceleration = 900.f;
	jogSetting.BrakingDeceleration = 900.f;
	jogSetting.BrakingFrictionFactor = 1.0f;
	jogSetting.BrakingFriction = 1.0f;
	jogSetting.UseSeperateBrakingFriction = true;
	GateSetting.Add(EGate::EG_Jog, jogSetting);

	FGateSetting sprintSetting;
	sprintSetting.MaxMoveSpeed = 800.f;
	sprintSetting.MaxAcceleration = 1600.f;
	sprintSetting.BrakingDeceleration = 2400.f;
	sprintSetting.BrakingFrictionFactor = 1.0f;
	sprintSetting.BrakingFriction = 0.5f;
	sprintSetting.UseSeperateBrakingFriction = true;
	GateSetting.Add(EGate::EG_Sprint, sprintSetting);	
}


// Called every frame
void UShooterMovementHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UShooterMovementHandler::BindMovementHandler(AShooterCharacter* TargetCharacter)
{
	ShooterCharacter = TargetCharacter;
	ShooterCharacter->MovementHandler = this;

	// Configure character movement
	ShooterCharacter->GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	ShooterCharacter->GetCharacterMovement()->RotationRate = FRotator(0.0f, 800.0f, 0.0f); // ...at this rotation rate
	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	ShooterCharacter->GetCharacterMovement()->JumpZVelocity = 1000.f;
	ShooterCharacter->GetCharacterMovement()->GravityScale = 3.f;
	ShooterCharacter->GetCharacterMovement()->AirControl = 0.15f;

	ShooterCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseMovespeed;
	ShooterCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = 200.f;
	ShooterCharacter->GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	ShooterCharacter->GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	ShooterCharacter->GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Active movement
	EnableMovementAction();

	UpdateGate(EGate::EG_Jog);
}

void UShooterMovementHandler::EnableMovementAction()
{
	// Check that the character is valid, and has no weapon component yet
	if (ShooterCharacter == nullptr || ShooterCharacter->GetInstanceComponents().FindItemByClass<UShooterMovementHandler>()) return;

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(ShooterCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(IMC_MovementHandler, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &UShooterMovementHandler::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &UShooterMovementHandler::StopJumping);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UShooterMovementHandler::Move);
			EnhancedInputComponent->BindAction(StanceAction, ETriggerEvent::Started, this, &UShooterMovementHandler::Crouch);
			EnhancedInputComponent->BindAction(StanceAction, ETriggerEvent::Completed, this, &UShooterMovementHandler::UnCrouch);
			EnhancedInputComponent->BindAction(GaitAction, ETriggerEvent::Started, this, &UShooterMovementHandler::Sprint);
			EnhancedInputComponent->BindAction(GaitAction, ETriggerEvent::Completed, this, &UShooterMovementHandler::Jog);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &UShooterMovementHandler::AimHold);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &UShooterMovementHandler::AimRelease);
		}
	}
}

void UShooterMovementHandler::DisableMovementAction()
{
	if (ShooterCharacter != nullptr)
	{
		// remove the input mapping context from the Player Controller
		if (APlayerController* PlayerController = Cast<APlayerController>(ShooterCharacter->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(IMC_MovementHandler);
			}
		}
	}
}

void UShooterMovementHandler::UpdateGate(EGate Gate)
{
	CurrentGate = Gate;
	FGateSetting* gateSetting = GateSetting.Find(CurrentGate);
	
	UCharacterMovementComponent* movement = ShooterCharacter->GetCharacterMovement();
	movement->MaxWalkSpeed = gateSetting->MaxMoveSpeed;
	movement->MaxAcceleration = gateSetting->MaxAcceleration;
	movement->BrakingDecelerationWalking = gateSetting->BrakingDeceleration;
	movement->BrakingFrictionFactor = gateSetting->BrakingFrictionFactor;
	movement->BrakingFriction = gateSetting->BrakingFriction;
	movement->bUseSeparateBrakingFriction = gateSetting->UseSeperateBrakingFriction;
	switch (CurrentGate)
	{
	case EGate::EG_Walk:
		UE_LOG(LogTemp, Warning, TEXT("EG_Walk"));
		break;
	case EGate::EG_Jog:
		UE_LOG(LogTemp, Warning, TEXT("EG_Jog"));
		break;
	case EGate::EG_Sprint:
		UE_LOG(LogTemp, Warning, TEXT("EG_Sprint"));
		break;
	case EGate::EG_MAX:
		break;
	default:
		break;
	}
	ShooterCharacter->GetCharacterAnimInstance()->ReceiveCurrentGate(CurrentGate);
	ShooterCharacter->OnChangeGateDelegate.Broadcast(CurrentGate);
}

void UShooterMovementHandler::Move(const FInputActionValue& Value)
{
	if (!ShooterCharacter || !ShooterCharacter->GetController()) return;

	FVector2D MovementVector = Value.Get<FVector2D>();


	if (ShooterCharacter->Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = ShooterCharacter->Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		ShooterCharacter->AddMovementInput(ForwardDirection, MovementVector.Y);
		ShooterCharacter->AddMovementInput(RightDirection, MovementVector.X);
	}
}

void UShooterMovementHandler::Jump()
{
	ShooterCharacter->Jump();
}

void UShooterMovementHandler::StopJumping()
{
	ShooterCharacter->StopJumping();
}

void UShooterMovementHandler::Crouch()
{
	UpdateGate(EGate::EG_Walk);
	bIsCrouched = true;
	ShooterCharacter->Crouch();
}

void UShooterMovementHandler::UnCrouch()
{
	UpdateGate(EGate::EG_Jog);
	bIsCrouched = false;
	ShooterCharacter->UnCrouch();

}

void UShooterMovementHandler::Dodge()
{
	
}

void UShooterMovementHandler::Sprint()
{
	UpdateGate(EGate::EG_Sprint);
}

void UShooterMovementHandler::Jog()
{
	UpdateGate(EGate::EG_Jog);
}

void UShooterMovementHandler::AimHold()
{
	UpdateGate(EGate::EG_Walk);
	bIsAimed = true;
	
	ShooterCharacter->bUseControllerRotationYaw = true;
}

void UShooterMovementHandler::AimRelease()
{
	UpdateGate(EGate::EG_Jog);
	bIsAimed = false;
	if (EEquippedState::EES_Unarmed == ShooterCharacter->GetCharacterAnimInstance()->EquippedState)
	{
		ShooterCharacter->bUseControllerRotationYaw = false;
	}	
}

void UShooterMovementHandler::CameraSwap()
{
}

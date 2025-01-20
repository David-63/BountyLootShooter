// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterMovementHandler.h"
#include "ShooterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ShooterAnimInstance.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"


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

	
}


// Called every frame
void UShooterMovementHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...	

	//UKismetSystemLibrary::SphereTraceMulti
	
	TickTransition(DeltaTime);
	CheckGround();
}

void UShooterMovementHandler::CheckGround()
{
	float capsuleHeight = ShooterCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector characterLocation = ShooterCharacter->GetActorLocation();

	FVector groundTraceStartLocation = characterLocation;
	FVector groundTraceEndLocation = characterLocation;

	groundTraceStartLocation.Z -= capsuleHeight;
	groundTraceEndLocation.Z -= 1000.f;

	FHitResult outHit;
	TArray<AActor*> actorsToIgnore;
	EDrawDebugTrace::Type drawDebugType = EDrawDebugTrace::ForOneFrame;

	UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(), groundTraceStartLocation, groundTraceEndLocation, 5.f,
		UEngineTypes::ConvertToTraceType(ECC_Visibility), false, actorsToIgnore, drawDebugType, outHit, true);
	ShooterCharacter->GetCharacterAnimInstance()->ReceiveGroundDistance(outHit.Distance);
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
	ShooterCharacter->GetCharacterMovement()->JumpZVelocity = 700.f;
	ShooterCharacter->GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	ShooterCharacter->GetCharacterMovement()->AirControl = 0.25f;
	ShooterCharacter->GetCharacterMovement()->GravityScale = 2.f;

	ShooterCharacter->GetCharacterMovement()->MaxWalkSpeed = 300.f;
	ShooterCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = 250.f;
	ShooterCharacter->GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	ShooterCharacter->GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

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
	ShooterCharacter->GetCharacterAnimInstance()->ReceiveCurrentGate(CurrentGate);

	if (nullptr == GateDataTable)
		return;


	UCharacterMovementComponent* movement = ShooterCharacter->GetCharacterMovement();	

	EEquippedState equipState = ShooterCharacter->GetEquippedState();

	static const FString ContextString(TEXT("GENERAL"));
	FGateMovement* gateMovement = nullptr;
	switch (CurrentGate)
	{
	case EGate::EG_Walk:
		switch (equipState)
		{
		case EEquippedState::EES_TwoHands:
			gateMovement = GateDataTable->FindRow<FGateMovement>(RifleWalk, ContextString);
			break;
		case EEquippedState::EES_OneHand:
			gateMovement = GateDataTable->FindRow<FGateMovement>(PistolWalk, ContextString);
			break;
		case EEquippedState::EES_Unarmed:
			gateMovement = GateDataTable->FindRow<FGateMovement>(UnarmedWalk, ContextString);
			break;
		}
		break;
	case EGate::EG_Jog:
		switch (equipState)
		{
		case EEquippedState::EES_TwoHands:
			gateMovement = GateDataTable->FindRow<FGateMovement>(RifleJog, ContextString);
			break;
		case EEquippedState::EES_OneHand:
			gateMovement = GateDataTable->FindRow<FGateMovement>(PistolJog, ContextString);
			break;
		case EEquippedState::EES_Unarmed:
			gateMovement = GateDataTable->FindRow<FGateMovement>(UnarmedJog, ContextString);
			break;
		}
		break;
	case EGate::EG_Sprint:
		switch (equipState)
		{
		case EEquippedState::EES_TwoHands:
			gateMovement = GateDataTable->FindRow<FGateMovement>(RifleSprint, ContextString);
			break;
		case EEquippedState::EES_OneHand:
			gateMovement = GateDataTable->FindRow<FGateMovement>(PistolSprint, ContextString);
			break;
		case EEquippedState::EES_Unarmed:
			gateMovement = GateDataTable->FindRow<FGateMovement>(UnarmedSprint, ContextString);
			break;
		}
		break;
	case EGate::EG_Crouch:
		switch (equipState)
		{
		case EEquippedState::EES_TwoHands:
			gateMovement = GateDataTable->FindRow<FGateMovement>(RifleCrouch, ContextString);
			break;
		case EEquippedState::EES_OneHand:
			gateMovement = GateDataTable->FindRow<FGateMovement>(PistolCrouch, ContextString);
			break;
		case EEquippedState::EES_Unarmed:
			gateMovement = GateDataTable->FindRow<FGateMovement>(UnarmedCrouch, ContextString);
			break;
		}		
		break;
	}

	if (gateMovement)
	{
		UE_LOG(LogTemp, Warning, TEXT("GateChange"));

		movement->MaxWalkSpeed = gateMovement->MaxMoveSpeed;
		movement->MaxAcceleration = gateMovement->MaxAcceleration;
		movement->BrakingDecelerationWalking = gateMovement->BrakingDeceleration;
		movement->BrakingFrictionFactor = gateMovement->BrakingFrictionFactor;
		movement->BrakingFriction = gateMovement->BrakingFriction;
		movement->bUseSeparateBrakingFriction = gateMovement->UseSeperateBrakingFriction;

		if (EGate::EG_Crouch == CurrentGate)
		{
			movement->MaxWalkSpeedCrouched = gateMovement->MaxMoveSpeed;
		}
	}
}

void UShooterMovementHandler::BeginTransition()
{
	// 타이머 초기화하고
	TransitionTimeCur = 0.f;
	// 트렌지션 시작
	DoTransition = true;
}

void UShooterMovementHandler::TickTransition(float DeltaTime)
{
	if (!DoTransition) return;

	float endTime = 0.15f;
	float alpha = FMath::Clamp(TransitionTimeCur / endTime, 0.f, 1.f);
	TransitionTimeCur += DeltaTime;

	// 완료 조건
	if (TransitionTimeCur >= endTime)
	{
		DoTransition = false;
		return;
	}

	// lerp 적용
	if (bIsAimed)
	{
		ShooterCharacter->GetTpsSpringArm()->TargetArmLength = FMath::Lerp(DefaultArmLength, AimArmLength, alpha); 
	}
	else
	{
		ShooterCharacter->GetTpsSpringArm()->TargetArmLength = FMath::Lerp(AimArmLength, DefaultArmLength, alpha);
	}
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
	ShooterCharacter->Crouch();
	UpdateGate(EGate::EG_Crouch);
	bIsCrouched = true;
}

void UShooterMovementHandler::UnCrouch()
{
	ShooterCharacter->UnCrouch();
	UpdateGate(EGate::EG_Jog);
	bIsCrouched = false;

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
	if (EGate::EG_Crouch == CurrentGate)
	{
		UpdateGate(EGate::EG_Crouch);
	}
	else
	{
		UpdateGate(EGate::EG_Walk);
	}
	BeginTransition();
	bIsAimed = true;
}

void UShooterMovementHandler::AimRelease()
{
	
	if (EGate::EG_Crouch == CurrentGate)
	{
		UpdateGate(EGate::EG_Crouch);
	}
	else
	{
		UpdateGate(EGate::EG_Jog);
	}
	BeginTransition();
	bIsAimed = false;
}

void UShooterMovementHandler::CameraSwap()
{
}

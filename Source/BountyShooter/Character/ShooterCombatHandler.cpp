// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCombatHandler.h"
#include "ShooterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UShooterCombatHandler::UShooterCombatHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UShooterCombatHandler::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UShooterCombatHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UShooterCombatHandler::MappingCombatContext(AShooterCharacter* TargetCharacter)
{
	ShooterCharacter = TargetCharacter;
	ShooterCharacter->CombatHandler = this;

	// Check that the character is valid, and has no weapon component yet
	if (ShooterCharacter == nullptr || ShooterCharacter->GetInstanceComponents().FindItemByClass<UShooterMovementHandler>()) return;

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(ShooterCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_CombatHandler, 2);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UShooterCombatHandler::Fire);
			EnhancedInputComponent->BindAction(MeleeAction, ETriggerEvent::Triggered, this, &UShooterCombatHandler::Melee);
			EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &UShooterCombatHandler::Throw);
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &UShooterCombatHandler::ChamberingRound);
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &UShooterCombatHandler::AmmoInsertion);
			
		}
	}
}

void UShooterCombatHandler::ChamberingRound()
{
}

void UShooterCombatHandler::AmmoInsertion()
{
}

void UShooterCombatHandler::Fire()
{
	// 나중에 무기에서 제공하는 함수를 호출하기만 할 예정
	
	// 시작지점
	FVector beginPoint = ShooterCharacter->GetActorLocation();
	beginPoint.Z += 60.f;

	// 종료지점	
	DrawDebugLine(GetWorld(), beginPoint, ShooterCharacter->GetHitLocation(), FColor::Red, false, -1, 0, 12.333);
}

void UShooterCombatHandler::Melee()
{
}

void UShooterCombatHandler::Throw()
{
}


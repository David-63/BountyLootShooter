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
	FHitResult result;

	
	// center location
	FVector2D viewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(viewportSize);
	}
	FVector2D crossHairLocation(viewportSize.X / 2.f, viewportSize.Y / 2.f);




	// Increase the size of inertia by multiplying inertiaMagnitude
	//FVector2D inertiaValue = Character->GetInertiaValue() * InertiaMagnitude;
	//crossHairLocation.X += inertiaValue.X;
	//crossHairLocation.Y += inertiaValue.Y;

	// Calculate inverse matrix from the crosshair position with inertia applied | screen -> World
	FVector crossHairWorldPosition, crossHairWorldDirection;
	bool isScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
	(UGameplayStatics::GetPlayerController(this, 0), crossHairLocation, crossHairWorldPosition, crossHairWorldDirection);

	if (!isScreenToWorld) return;

	FVector begin = crossHairWorldPosition;

	if (ShooterCharacter)
	{
		float distanceToCharacter = (ShooterCharacter->GetActorLocation() - begin).Size();
		begin += crossHairWorldDirection * (distanceToCharacter + 100.f);
	}
	FVector end = begin + crossHairWorldDirection * TRACE_LENGTH;


	// Ray result return
	GetWorld()->LineTraceSingleByChannel(result, begin, end, ECollisionChannel::ECC_Visibility);

	// crosshair color change
	/*if (_traceHitResult.GetActor() && _traceHitResult.GetActor()->Implements<UCrosshairInteractor>())
	{
		CrosshairPackage.CrosshairsColor = FLinearColor::Yellow;
	}
	else
	{
		CrosshairPackage.CrosshairsColor = FLinearColor::White;
	}*/

	if (!result.bBlockingHit)
	{
		result.ImpactPoint = end;
	}


	HitTarget = end;










	// ...
}

void UShooterCombatHandler::MappingCombatContext(AShooterCharacter* TargetCharacter)
{
	ShooterCharacter = TargetCharacter;

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
	// 시작지점
	FVector beginPoint = ShooterCharacter->GetActorLocation();
	beginPoint.Z += 60.f;
	// 종료지점
	HitTarget;
	DrawDebugLine(GetWorld(), beginPoint, HitTarget, FColor::Red, false, -1, 0, 12.333);
}

void UShooterCombatHandler::Melee()
{
}

void UShooterCombatHandler::Throw()
{
}


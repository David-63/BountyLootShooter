// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCombatHandler.h"
#include "ShooterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

#include "ShooterInventoryHandler.h"
#include "BountyShooter/Items/Weapons/WeaponBase.h"
#include "BountyShooter/Items/Weapons/PlatformComponent.h"
#include "ShooterAnimInstance.h"

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

void UShooterCombatHandler::BindCombatHandler(AShooterCharacter* TargetCharacter)
{
	ShooterCharacter = TargetCharacter;
	ShooterCharacter->CombatHandler = this;
}

void UShooterCombatHandler::EnableCombatAction()
{
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
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Canceled, this, &UShooterCombatHandler::Reload);
			//EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &UShooterCombatHandler::AmmoInsertion);
		}
	}
}

void UShooterCombatHandler::DisableCombatAction()
{
	if (ShooterCharacter != nullptr)
	{
		// remove the input mapping context from the Player Controller
		if (APlayerController* PlayerController = Cast<APlayerController>(ShooterCharacter->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(IMC_CombatHandler);
			}
		}
	}
}

void UShooterCombatHandler::Reload()
{
	AWeaponBase* weapon = ShooterCharacter->InventoryHandler->GetSelectedWeapon();
	if (!weapon) return;
	ShooterCharacter->PlayAnimMontage(weapon->GetPlatformComponent()->GetReloadMontage(), 1.f);
	weapon->GetPlatformComponent()->PlayReloadMontage();
	weapon->Reload();
}

void UShooterCombatHandler::ChamberingRound()
{
	
}

void UShooterCombatHandler::AmmoInsertion()
{
}

void UShooterCombatHandler::Fire()
{
	// 제어 조건

	EGate currentGate = ShooterCharacter->GetCharacterAnimInstance()->GetCurrentGate();

	if (EGate::EG_Sprint == currentGate)
	{
		return;
	}
	
	AWeaponBase* weapon = ShooterCharacter->InventoryHandler->GetSelectedWeapon();
	if (!weapon) return;
	
	if (CanFire())
	{
		FireTimerStart();
		ShooterCharacter->PlayAnimMontage(weapon->GetPlatformComponent()->GetFireMontage(), 1.f);
		weapon->GetPlatformComponent()->PlayFireMontage();
		weapon->FireRound(ShooterCharacter->GetHitLocation());
	}
}

void UShooterCombatHandler::Melee()
{
}

void UShooterCombatHandler::Throw()
{
}

void UShooterCombatHandler::FireTimerStart()
{
	AWeaponBase* weapon = ShooterCharacter->InventoryHandler->GetSelectedWeapon();
	if (ShooterCharacter && weapon)
	{
		bFireCooldownFinished = false;
		float totalFireRate = 1 / weapon->GetFireRate();
		ShooterCharacter->GetWorldTimerManager().SetTimer(FireTimer, this, &UShooterCombatHandler::FireTimerFinished, totalFireRate);
	}
}

void UShooterCombatHandler::FireTimerFinished()
{
	bFireCooldownFinished = true;
	if (bIsAttackDown)
	{
		Fire();
	}
}

bool UShooterCombatHandler::CanFire()
{
	AWeaponBase* weapon = ShooterCharacter->InventoryHandler->GetSelectedWeapon();

	const bool bChamberEnable = weapon->IsChamberEnable();
	const bool bWeaponReady = bFireCooldownFinished && bChamberEnable;

	return bWeaponReady;
}



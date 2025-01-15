// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterInventoryHandler.h"

#include "ShooterCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "ShooterAnimInstance.h"


#include "BountyShooter/Items/Weapons/WeaponBase.h"

// Sets default values for this component's properties
UShooterInventoryHandler::UShooterInventoryHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UShooterInventoryHandler::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UShooterInventoryHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UShooterInventoryHandler::BindInventoryHandler(AShooterCharacter* TargetCharacter)
{
	ShooterCharacter = TargetCharacter;
	ShooterCharacter->InventoryHandler = this;

	EnableInventoryAction();

	WeaponSlots.Add(ELoadoutSlot::ELS_Primary, nullptr);
	WeaponSlots.Add(ELoadoutSlot::ELS_Secondary, nullptr);
	WeaponSlots.Add(ELoadoutSlot::ELS_Sidearm, nullptr);

	SelectedWeaponSlot = ELoadoutSlot::ELS_Primary;

}

void UShooterInventoryHandler::EnableInventoryAction()
{
	// Check that the character is valid, and has no weapon component yet
	if (ShooterCharacter == nullptr || ShooterCharacter->GetInstanceComponents().FindItemByClass<UShooterMovementHandler>()) return;

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(ShooterCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(IMC_InventoryHandler, 3);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(WeaponSwapAction, ETriggerEvent::Triggered, this, &UShooterInventoryHandler::WeaponSwap);
			EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Triggered, this, &UShooterInventoryHandler::ToggleWeapon);
			EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Triggered, this, &UShooterInventoryHandler::SelectWeaponDrop);
		}
	}	
}

void UShooterInventoryHandler::DisableInventoryAction()
{
	if (ShooterCharacter != nullptr)
	{
		// remove the input mapping context from the Player Controller
		if (APlayerController* PlayerController = Cast<APlayerController>(ShooterCharacter->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(IMC_InventoryHandler);
			}
		}
	}
}


void UShooterInventoryHandler::WeaponSwap(const FInputActionValue& Value)
{
	float ScalarValue = Value.Get<float>();
	uint32 slot = static_cast<uint32>(ScalarValue);
	switch (slot)
	{
	case 1:
		SwapWeapon(ELoadoutSlot::ELS_Primary);
		break;
	case 2:
		SwapWeapon(ELoadoutSlot::ELS_Secondary);
		break;
	case 3:
		SwapWeapon(ELoadoutSlot::ELS_Sidearm);
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid weapon slot: %f"), ScalarValue);
		break;
	}
	//ShooterCharacter->GetCharacterAnimInstance()->RecieveEquippedState(SelectedWeaponSlot);
}


void UShooterInventoryHandler::ToggleWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("Hold E called ToggleWeapon function"));
	if (!isWeaponArmed)
	{
		// 현재 슬롯에 무기가 있으면 꺼내고
		if (DrawSelectWeapon())
		{
			switch (SelectedWeaponSlot)
			{
			case ELoadoutSlot::ELS_Primary:
			case ELoadoutSlot::ELS_Secondary:
				ShooterCharacter->OnWeaponSwapDelegate.Broadcast(EEquippedState::EES_TwoHands);
				ShooterCharacter->GetCharacterAnimInstance()->ReceiveEquippedState(EEquippedState::EES_TwoHands);
				UE_LOG(LogTemp, Warning, TEXT("Broadcast TwoHands"));
				break;
			case ELoadoutSlot::ELS_Sidearm:
				ShooterCharacter->OnWeaponSwapDelegate.Broadcast(EEquippedState::EES_OneHand);
				ShooterCharacter->GetCharacterAnimInstance()->ReceiveEquippedState(EEquippedState::EES_OneHand);
				UE_LOG(LogTemp, Warning, TEXT("Broadcast OneHand"));
				break;
			case ELoadoutSlot::ELS_MAX:
				ShooterCharacter->OnWeaponSwapDelegate.Broadcast(EEquippedState::EES_Unarmed);
				ShooterCharacter->GetCharacterAnimInstance()->ReceiveEquippedState(EEquippedState::EES_Unarmed);
				UE_LOG(LogTemp, Warning, TEXT("Broadcast Unarmed"));
				break;
			default:
				break;
			}
		}
		else
		{
			ELoadoutSlot otherSlot = GetOccupiedWeaponSlot();
			if (ELoadoutSlot::ELS_MAX == otherSlot) return;

			WeaponSlots[otherSlot]->DrawWeapon(FName(TEXT("WeaponEquipped")));
			switch (otherSlot)
			{
			case ELoadoutSlot::ELS_Primary:
			case ELoadoutSlot::ELS_Secondary:
				ShooterCharacter->OnWeaponSwapDelegate.Broadcast(EEquippedState::EES_TwoHands);
				UE_LOG(LogTemp, Warning, TEXT("Broadcast TwoHands"));
				break;
			case ELoadoutSlot::ELS_Sidearm:
				ShooterCharacter->OnWeaponSwapDelegate.Broadcast(EEquippedState::EES_OneHand);
				UE_LOG(LogTemp, Warning, TEXT("Broadcast OneHand"));
				break;
			case ELoadoutSlot::ELS_MAX:
				ShooterCharacter->OnWeaponSwapDelegate.Broadcast(EEquippedState::EES_Unarmed);
				UE_LOG(LogTemp, Warning, TEXT("Broadcast Unarmed"));
				break;
			default:
				break;
			}
		}
	}
	else
	{		
		// 무기 집어넣기
		HolsterSelectWeapon();
		
		ShooterCharacter->OnWeaponSwapDelegate.Broadcast(EEquippedState::EES_Unarmed);
		UE_LOG(LogTemp, Warning, TEXT("Broadcast Unarmed"));
		//ShooterCharacter->GetCharacterAnimInstance()->RecieveEquippedState(ELoadoutSlot::ELS_MAX);
	}
}

void UShooterInventoryHandler::SelectWeaponDrop()
{
	HolsterSelectWeapon();
	if (WeaponSlots[SelectedWeaponSlot])
	{
		WeaponSlots[SelectedWeaponSlot]->Drop();
		WeaponSlots[SelectedWeaponSlot] = nullptr;
		SelectedWeaponSlot = ELoadoutSlot::ELS_MAX;
	}
}



bool UShooterInventoryHandler::DrawSelectWeapon()
{
	if (WeaponSlots[SelectedWeaponSlot])
	{
		isWeaponArmed = true;
		WeaponSlots[SelectedWeaponSlot]->DrawWeapon(FName(TEXT("WeaponEquipped")));
		ShooterCharacter->EnableCombatAction();
		return true;
	}
	return false;
}
bool UShooterInventoryHandler::HolsterSelectWeapon()
{
	if (WeaponSlots[SelectedWeaponSlot])
	{
		isWeaponArmed = false;
		WeaponSlots[SelectedWeaponSlot]->HolsterWeapon(GetHolsterSocketName());
		ShooterCharacter->DisableCombatAction();
		return true;
	}
	return false;
}

ELoadoutSlot UShooterInventoryHandler::GetOccupiedWeaponSlot()
{
	if (WeaponSlots[ELoadoutSlot::ELS_Primary])
	{
		return ELoadoutSlot::ELS_Primary;
	}
	
	if (WeaponSlots[ELoadoutSlot::ELS_Secondary])
	{
		return ELoadoutSlot::ELS_Secondary;
	}
		
	if (WeaponSlots[ELoadoutSlot::ELS_Sidearm])
	{
		return ELoadoutSlot::ELS_Sidearm;
	}

	return ELoadoutSlot::ELS_MAX;
}

void UShooterInventoryHandler::PickupItem(AItemBase* Item)
{
	HolsterSelectWeapon();
	Item->Equip(ShooterCharacter);
}


ELoadoutSlot UShooterInventoryHandler::FindEmptyWeaponSlot(EWeaponCategory WeaponCategory)
{
	TObjectPtr<AWeaponBase>* targetItem = nullptr;
	
	if (EWeaponCategory::EWC_Large == WeaponCategory)
	{
		targetItem = WeaponSlots.Find(ELoadoutSlot::ELS_Primary);
		if (!*targetItem)
		{
			return ELoadoutSlot::ELS_Primary;
		}
		targetItem = WeaponSlots.Find(ELoadoutSlot::ELS_Secondary);
		if (!*targetItem)
		{
			return ELoadoutSlot::ELS_Secondary;
		}
	}
	
	else if (EWeaponCategory::EWC_Compact == WeaponCategory)
	{
		targetItem = WeaponSlots.Find(ELoadoutSlot::ELS_Sidearm);
		if (!*targetItem)
		{
			return ELoadoutSlot::ELS_Sidearm;
		}
	}

	return ELoadoutSlot::ELS_MAX;
}

void UShooterInventoryHandler::BindWeaponSlot(AWeaponBase* Item, ELoadoutSlot EquippableSlot)
{
	// 기존 무기 집어놓고
	HolsterSelectWeapon();

	// 무기 할당해 준 다음에
	WeaponSlots[EquippableSlot] = Item;
	SelectedWeaponSlot = EquippableSlot;

	// 현재 슬롯을 변경해주고 무기 꺼내기
	if (DrawSelectWeapon())
	{
		switch (SelectedWeaponSlot)
		{
		case ELoadoutSlot::ELS_Primary:
		case ELoadoutSlot::ELS_Secondary:
			ShooterCharacter->OnWeaponSwapDelegate.Broadcast(EEquippedState::EES_TwoHands);
			UE_LOG(LogTemp, Warning, TEXT("Broadcast TwoHands"));
			break;
		case ELoadoutSlot::ELS_Sidearm:
			ShooterCharacter->OnWeaponSwapDelegate.Broadcast(EEquippedState::EES_OneHand);
			UE_LOG(LogTemp, Warning, TEXT("Broadcast OneHand"));
			break;
		case ELoadoutSlot::ELS_MAX:
			ShooterCharacter->OnWeaponSwapDelegate.Broadcast(EEquippedState::EES_Unarmed);
			UE_LOG(LogTemp, Warning, TEXT("Broadcast Unarmed"));
			break;
		default:
			break;
		}
	}
}
void UShooterInventoryHandler::ReplaceWeaponSlot(AWeaponBase* Item, ELoadoutSlot EquippableSlot)
{
	// 현재 사용중인 무기 집어넣고
	SelectWeaponDrop();

	// 슬롯에 아이템 넣기
	WeaponSlots[SelectedWeaponSlot] = Item;

	if (DrawSelectWeapon())
	{
		switch (SelectedWeaponSlot)
		{
		case ELoadoutSlot::ELS_Primary:
		case ELoadoutSlot::ELS_Secondary:
			ShooterCharacter->OnWeaponSwapDelegate.Broadcast(EEquippedState::EES_TwoHands);
			UE_LOG(LogTemp, Warning, TEXT("Broadcast TwoHands"));
			break;
		case ELoadoutSlot::ELS_Sidearm:
			ShooterCharacter->OnWeaponSwapDelegate.Broadcast(EEquippedState::EES_OneHand);
			UE_LOG(LogTemp, Warning, TEXT("Broadcast OneHand"));
			break;
		case ELoadoutSlot::ELS_MAX:
			ShooterCharacter->OnWeaponSwapDelegate.Broadcast(EEquippedState::EES_Unarmed);
			UE_LOG(LogTemp, Warning, TEXT("Broadcast Unarmed"));
			break;
		default:
			break;
		}
	}
}




void UShooterInventoryHandler::SwapWeapon(ELoadoutSlot NextWeaponSlot)
{
	if (!WeaponSlots[NextWeaponSlot]) return;
	
	HolsterSelectWeapon();
	SelectedWeaponSlot = NextWeaponSlot;
	if (DrawSelectWeapon())
	{
		switch (SelectedWeaponSlot)
		{
		case ELoadoutSlot::ELS_Primary:
		case ELoadoutSlot::ELS_Secondary:
			ShooterCharacter->OnWeaponSwapDelegate.Broadcast(EEquippedState::EES_TwoHands);
			UE_LOG(LogTemp, Warning, TEXT("Broadcast TwoHands"));
			break;
		case ELoadoutSlot::ELS_Sidearm:
			ShooterCharacter->OnWeaponSwapDelegate.Broadcast(EEquippedState::EES_OneHand);
			UE_LOG(LogTemp, Warning, TEXT("Broadcast OneHand"));
			break;
		case ELoadoutSlot::ELS_MAX:
			ShooterCharacter->OnWeaponSwapDelegate.Broadcast(EEquippedState::EES_Unarmed);
			UE_LOG(LogTemp, Warning, TEXT("Broadcast Unarmed"));
			break;
		default:
			break;
		}
	}
}



AWeaponBase* UShooterInventoryHandler::GetSelectedWeapon()
{
	TObjectPtr<AWeaponBase>* targetItem = WeaponSlots.Find(SelectedWeaponSlot);
	return targetItem ? *targetItem : nullptr;
}

FName UShooterInventoryHandler::GetHolsterSocketName()
{
	FName socketName;
	switch (SelectedWeaponSlot)
	{
	case ELoadoutSlot::ELS_Primary:
		socketName = FName(TEXT("Primary"));
		break;
	case ELoadoutSlot::ELS_Secondary:
		socketName = FName(TEXT("Secondary"));
		break;
	case ELoadoutSlot::ELS_Sidearm:
		socketName = FName(TEXT("Sidearm"));
		break;
	}
	return socketName;
}

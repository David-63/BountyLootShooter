// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterInventoryHandler.h"

#include "ShooterCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"



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

	WeaponSlots.Add(EInventorySlot::EIS_Primary, nullptr);
	WeaponSlots.Add(EInventorySlot::EIS_Secondary, nullptr);
	WeaponSlots.Add(EInventorySlot::EIS_Sidearm, nullptr);

	SelectedWeaponSlot = EInventorySlot::EIS_Primary;

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
			EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Started, this, &UShooterInventoryHandler::SelectPrimary);
			EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Started, this, &UShooterInventoryHandler::SelectSecondary);
			EnhancedInputComponent->BindAction(SidarmAction, ETriggerEvent::Started, this, &UShooterInventoryHandler::SelectSidarm);
			EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Triggered, this, &UShooterInventoryHandler::ToggleWeapon);
			;
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

void UShooterInventoryHandler::SelectPrimary()
{
	SwapWeapon(EInventorySlot::EIS_Primary);
}

void UShooterInventoryHandler::SelectSecondary()
{
	SwapWeapon(EInventorySlot::EIS_Secondary);
}

void UShooterInventoryHandler::SelectSidarm()
{
	SwapWeapon(EInventorySlot::EIS_Sidearm);
}


void UShooterInventoryHandler::ToggleWeapon()
{

	if (!isWeaponArmed)
	{
		isWeaponArmed = true;
		// 현재 슬롯에 무기가 있으면 꺼내고
		if (WeaponSlots[SelectedWeaponSlot])
		{
			WeaponSlots[SelectedWeaponSlot]->DrawWeapon(FName(TEXT("Hand")));
			ShooterCharacter->EnableCombatAction();
		}
		// 없는경우 있는 무기 찾아서 슬롯 변경한 다음에 꺼내기
		else
		{
			EInventorySlot otherSlot = GetOccupiedWeaponSlot();
			if (EInventorySlot::EIS_MAX == otherSlot) return;

			WeaponSlots[otherSlot]->DrawWeapon(FName(TEXT("Hand")));
			ShooterCharacter->EnableCombatAction();
		}
	}
	else
	{
		isWeaponArmed = false;
		// 무기 집어넣기
		if (WeaponSlots[SelectedWeaponSlot])
		{
			WeaponSlots[SelectedWeaponSlot]->HolsterWeapon(GetHolsterSocketName());
			ShooterCharacter->DisableCombatAction();
		}
	}
}

EInventorySlot UShooterInventoryHandler::GetOccupiedWeaponSlot()
{
	if (WeaponSlots[EInventorySlot::EIS_Primary])
	{
		return EInventorySlot::EIS_Primary;
	}
	
	if (WeaponSlots[EInventorySlot::EIS_Secondary])
	{
		return EInventorySlot::EIS_Secondary;
	}
		
	if (WeaponSlots[EInventorySlot::EIS_Sidearm])
	{
		return EInventorySlot::EIS_Sidearm;
	}

	return EInventorySlot::EIS_MAX;
}




void UShooterInventoryHandler::EquipWeapon(AWeaponBase* Weapon)
{
	if (EWeaponCategory::EWC_MAX == Weapon->GetWeaponCategory()) return;
	// 플레이어 등록
	Weapon->BindOwner(ShooterCharacter);
	// 무기 빈자리 찾기
	EInventorySlot equippableSlot = FindEmptyWeaponSlot(Weapon->GetWeaponCategory());
	if (EInventorySlot::EIS_MAX != equippableSlot)
	{
		ReplaceWeapon(equippableSlot, Weapon);
	}
	else
	{
		equippableSlot = EWeaponCategory::EWC_Compact == Weapon->GetWeaponCategory()
			? EInventorySlot::EIS_Sidearm : SelectedWeaponSlot;

		ReplaceAndDestroyWeapon(equippableSlot, Weapon);		
	}
}


EInventorySlot UShooterInventoryHandler::FindEmptyWeaponSlot(EWeaponCategory WeaponCategory)
{
	TObjectPtr<AWeaponBase>* targetItem = nullptr;
	
	if (EWeaponCategory::EWC_Large == WeaponCategory)
	{
		targetItem = WeaponSlots.Find(EInventorySlot::EIS_Primary);
		if (!*targetItem)
		{
			return EInventorySlot::EIS_Primary;
		}
		targetItem = WeaponSlots.Find(EInventorySlot::EIS_Secondary);
		if (!*targetItem)
		{
			return EInventorySlot::EIS_Secondary;
		}
	}
	
	else if (EWeaponCategory::EWC_Compact == WeaponCategory)
	{
		targetItem = WeaponSlots.Find(EInventorySlot::EIS_Sidearm);
		if (!*targetItem)
		{
			return EInventorySlot::EIS_Sidearm;
		}
	}

	return EInventorySlot::EIS_MAX;
}

void UShooterInventoryHandler::ReplaceAndDestroyWeapon(EInventorySlot EquippableSlot, AWeaponBase* Weapon)
{
	if (WeaponSlots[EquippableSlot])
	{
		isWeaponArmed = false;
		WeaponSlots[EquippableSlot]->Destroy();
		WeaponSlots[EquippableSlot] = nullptr;
	}

	WeaponSlots[EquippableSlot] = Weapon;
	SelectedWeaponSlot = EquippableSlot;
	switch (SelectedWeaponSlot)
	{
	case EInventorySlot::EIS_Primary:
		UE_LOG(LogTemp, Warning, TEXT("EIS_Primary"));
		break;
	case EInventorySlot::EIS_Secondary:
		UE_LOG(LogTemp, Warning, TEXT("EIS_Secondary"));
		break;
	case EInventorySlot::EIS_Sidearm:
		UE_LOG(LogTemp, Warning, TEXT("EIS_Sidearm"));
		break;
	}
	if (WeaponSlots[SelectedWeaponSlot])
	{
		isWeaponArmed = true;
		WeaponSlots[SelectedWeaponSlot]->DrawWeapon(GetHolsterSocketName());
		ShooterCharacter->EnableCombatAction();
	}
}

void UShooterInventoryHandler::ReplaceWeapon(EInventorySlot EquippableSlot, AWeaponBase* Weapon)
{
	// 기존 무기 집어놓고
	if (WeaponSlots[SelectedWeaponSlot])
	{
		isWeaponArmed = false;
		WeaponSlots[SelectedWeaponSlot]->HolsterWeapon(GetHolsterSocketName());
	}
	// 무기 할당해 준 다음에
	WeaponSlots[EquippableSlot] = Weapon;
	// 현재 슬롯을 변경해주고 무기 꺼내기
	SelectedWeaponSlot = EquippableSlot;
	switch (SelectedWeaponSlot)
	{
	case EInventorySlot::EIS_Primary:
		UE_LOG(LogTemp, Warning, TEXT("EIS_Primary"));
		break;
	case EInventorySlot::EIS_Secondary:
		UE_LOG(LogTemp, Warning, TEXT("EIS_Secondary"));
		break;
	case EInventorySlot::EIS_Sidearm:
		UE_LOG(LogTemp, Warning, TEXT("EIS_Sidearm"));
		break;
	}

	
	if (WeaponSlots[SelectedWeaponSlot])
	{
		isWeaponArmed = true;
		WeaponSlots[SelectedWeaponSlot]->DrawWeapon(FName(TEXT("Hand")));
		ShooterCharacter->EnableCombatAction();
	}
}



void UShooterInventoryHandler::SwapWeapon(EInventorySlot NextWeaponSlot)
{
	if (NextWeaponSlot == SelectedWeaponSlot) return;

	
	if (WeaponSlots[SelectedWeaponSlot])
	{
		WeaponSlots[SelectedWeaponSlot]->HolsterWeapon(GetHolsterSocketName());
	}
	SelectedWeaponSlot = NextWeaponSlot;

	switch (SelectedWeaponSlot)
	{
	case EInventorySlot::EIS_Primary:
		UE_LOG(LogTemp, Warning, TEXT("EIS_Primary"));
		break;
	case EInventorySlot::EIS_Secondary:
		UE_LOG(LogTemp, Warning, TEXT("EIS_Secondary"));
		break;
	case EInventorySlot::EIS_Sidearm:
		UE_LOG(LogTemp, Warning, TEXT("EIS_Sidearm"));
		break;
	}

	if (WeaponSlots[SelectedWeaponSlot])
	{
		WeaponSlots[SelectedWeaponSlot]->DrawWeapon(FName(TEXT("Hand")));
		ShooterCharacter->EnableCombatAction();
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
	case EInventorySlot::EIS_Primary:
		socketName = FName(TEXT("Primary"));
		break;
	case EInventorySlot::EIS_Secondary:
		socketName = FName(TEXT("Secondary"));
		break;
	case EInventorySlot::EIS_Sidearm:
		socketName = FName(TEXT("Sidearm"));
		break;
	}
	return socketName;
}

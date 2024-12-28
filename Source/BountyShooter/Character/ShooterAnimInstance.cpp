// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UShooterAnimInstance::RecieveEquippedState(ELoadoutSlot Loadout)
{
	EEquippedState equippedState = EEquippedState::EES_Unarmed;
	switch (Loadout)
	{
	case ELoadoutSlot::ELS_Primary:
		equippedState = EEquippedState::EES_TwoHands;
		UE_LOG(LogTemp, Warning, TEXT("TwoHands"));
		break;
	case ELoadoutSlot::ELS_Secondary:
		equippedState = EEquippedState::EES_TwoHands;
		UE_LOG(LogTemp, Warning, TEXT("TwoHands"));
		break;
	case ELoadoutSlot::ELS_Sidearm:
		equippedState = EEquippedState::EES_OneHand;
		UE_LOG(LogTemp, Warning, TEXT("OneHand"));
		break;
	default:
		equippedState = EEquippedState::EES_Unarmed;
		UE_LOG(LogTemp, Warning, TEXT("Unarmed"));
		break;
	}
	EquippedState = equippedState;
}

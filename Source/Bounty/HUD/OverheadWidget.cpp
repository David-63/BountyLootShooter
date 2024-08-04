// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}

void UOverheadWidget::SetDisplayText(FString _textToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(_textToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* _inPawn)
{
	ENetRole localRole = _inPawn->GetLocalRole();
	FString role;
	switch (localRole)
	{
	case ROLE_None:
		role = FString("None");
		break;
	case ROLE_SimulatedProxy:
		role = FString("SimulatedProxy");
		break;
	case ROLE_AutonomousProxy:
		role = FString("AutonomousProxy");
		break;
	case ROLE_Authority:
		role = FString("Authority");
		break;
	case ROLE_MAX:
		role = FString("MAX");
		break;
	default:
		break;
	}

	FString localRoleString = FString::Printf(TEXT("Local Role: %s"), *role);
	SetDisplayText(localRoleString);
}

void UOverheadWidget::ShowPlayerName(APawn* _inPawn)
{
	APlayerState* playerState = _inPawn->GetPlayerState();
	FString playerName;
	if (playerState)
	{
		playerName = playerState->GetPlayerName();
	}
	SetDisplayText(playerName);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}

void UOverheadWidget::SetDisplayText(FString _text)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(_text));
	}
}

void UOverheadWidget::ShowPlayerName(APawn* _pawn)
{
	APlayerState* playerState = _pawn->GetPlayerState();
	FString playerName;
	if (playerState)
	{
		playerName = playerState->GetPlayerName();
	}
	SetDisplayText(playerName);
}


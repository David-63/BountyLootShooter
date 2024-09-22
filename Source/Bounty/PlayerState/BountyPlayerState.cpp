// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyPlayerState.h"
#include "Bounty/Character/BountyCharacter.h"
#include "Bounty/PlayerController/BountyPlayerController.h"
#include "Net/UnrealNetwork.h"


void ABountyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABountyPlayerState, LifeLoss);
}

void ABountyPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Player = Player == nullptr ? Cast<ABountyCharacter>(GetPawn()) : Player;
	if (Player)
	{
		PlayerController = PlayerController == nullptr ? Cast<ABountyPlayerController>(Player->Controller) : PlayerController;
		if (PlayerController)
		{
			PlayerController->SetHUD_Score(GetScore());
		}
	}
	
}
void ABountyPlayerState::AddToScore(float _scoreAmount)
{
	SetScore(GetScore() + _scoreAmount);
	
	Player = Player == nullptr ? Cast<ABountyCharacter>(GetPawn()) : Player;	
	if (Player)
	{
		
		PlayerController = PlayerController == nullptr ? Cast<ABountyPlayerController>(Player->Controller) : PlayerController;
		if (PlayerController)
		{
			PlayerController->SetHUD_Score(GetScore());
		}
	}
}

void ABountyPlayerState::OnRep_LifeLoss()
{
	Player = Player == nullptr ? Cast<ABountyCharacter>(GetPawn()) : Player;
	if (Player)
	{
		PlayerController = PlayerController == nullptr ? Cast<ABountyPlayerController>(Player->Controller) : PlayerController;
		if (PlayerController)
		{
			PlayerController->SetHUD_LifeLoss(LifeLoss);
		}
	}
}
void ABountyPlayerState::AddToLifeLoss(int32 _lossAmount)
{
	LifeLoss += _lossAmount;

	Player = Player == nullptr ? Cast<ABountyCharacter>(GetPawn()) : Player;
	if (Player)
	{
		PlayerController = PlayerController == nullptr ? Cast<ABountyPlayerController>(Player->Controller) : PlayerController;
		if (PlayerController)
		{
			PlayerController->SetHUD_LifeLoss(LifeLoss);
		}
	}
}

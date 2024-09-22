// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyPlayerState.h"
#include "Bounty/Character/BountyCharacter.h"
#include "Bounty/PlayerController/BountyPlayerController.h"

void ABountyPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Player = Player == nullptr ? Cast<ABountyCharacter>(GetPawn()) : Player;
	if (Player)
	{
		PlayerController = PlayerController == nullptr ? Cast<ABountyPlayerController>(Player->Controller) : PlayerController;
		if (PlayerController)
		{
			PlayerController->SetHUD_Score(Score);
		}
	}
	
}

void ABountyPlayerState::AddToScore(float _scoreAmount)
{
	Score += _scoreAmount;

	Player = Player == nullptr ? Cast<ABountyCharacter>(GetPawn()) : Player;
	
	if (Player)
	{
		
		PlayerController = PlayerController == nullptr ? Cast<ABountyPlayerController>(Player->Controller) : PlayerController;
		if (PlayerController)
		{
			PlayerController->SetHUD_Score(Score);
		}
	}
}

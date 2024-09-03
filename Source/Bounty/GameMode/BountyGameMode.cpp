// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyGameMode.h"
#include "Bounty/Character/BountyCharacter.h"
#include "Bounty/PlayerController/BountyPlayerController.h"

void ABountyGameMode::PlayerEliminated(ABountyCharacter* _elimmedCharacter, ABountyPlayerController* _victimController, ABountyPlayerController* _attackerController)
{
	if (_elimmedCharacter)
	{
		_elimmedCharacter->Elim();
	}
	
}

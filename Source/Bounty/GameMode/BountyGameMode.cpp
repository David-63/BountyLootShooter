// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyGameMode.h"
#include "Bounty/Character/BountyCharacter.h"
#include "Bounty/PlayerController/BountyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Bounty/PlayerState/BountyPlayerState.h"

void ABountyGameMode::PlayerEliminated(ABountyCharacter* _elimmedCharacter, ABountyPlayerController* _victimController, ABountyPlayerController* _attackerController)
{
	ABountyPlayerState* attackerState = _attackerController ? Cast<ABountyPlayerState>(_attackerController->PlayerState) : nullptr;
	ABountyPlayerState* victimState = _victimController ? Cast<ABountyPlayerState>(_victimController->PlayerState) : nullptr;

	if (attackerState && attackerState != victimState)
	{
		attackerState->AddToScore(1.f);
	}

	if (_elimmedCharacter)
	{
		_elimmedCharacter->Elim();
	}
	
}

void ABountyGameMode::RequestRespawn(ACharacter* _elimmedCharacter, AController* _elimmedController)
{
	if (_elimmedCharacter)
	{
		_elimmedCharacter->Reset();
		_elimmedCharacter->Destroy();
	}
	if (_elimmedController)
	{
		TArray<AActor*> playerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), playerStarts);
		int32 selection = FMath::RandRange(0, playerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(_elimmedController, playerStarts[selection]);
	}
}

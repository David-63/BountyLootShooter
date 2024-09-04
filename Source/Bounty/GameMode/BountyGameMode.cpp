// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyGameMode.h"
#include "Bounty/Character/BountyCharacter.h"
#include "Bounty/PlayerController/BountyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

void ABountyGameMode::PlayerEliminated(ABountyCharacter* _elimmedCharacter, ABountyPlayerController* _victimController, ABountyPlayerController* _attackerController)
{
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

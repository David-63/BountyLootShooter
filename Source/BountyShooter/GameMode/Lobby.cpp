// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobby::PostLogin(APlayerController* _newPlayer)
{
	Super::PostLogin(_newPlayer);

	APlayerState* playerState = _newPlayer->GetPlayerState<APlayerState>();
	if (playerState)
	{
		FString playerName = playerState->GetPlayerName();
		GEngine->AddOnScreenDebugMessage(
			5,
			60.f,
			FColor::Cyan,
			FString::Printf(TEXT("%s has joined the game!"), *playerName)
		);
	}

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if (NumberOfPlayers == 2)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			bUseSeamlessTravel = true;
			World->ServerTravel(FString("/Game/BountyContents/Maps/PlayGround?listen"));

		}
	}
}

void ALobby::Logout(AController* _exiting)
{
	Super::Logout(_exiting);

	APlayerState* playerState = _exiting->GetPlayerState<APlayerState>();
	if (playerState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		GEngine->AddOnScreenDebugMessage(
			5,
			600.f,
			FColor::Yellow,
			FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers - 1)
		);

		FString playerName = playerState->GetPlayerName();
		GEngine->AddOnScreenDebugMessage(
			5,
			60.f,
			FColor::Cyan,
			FString::Printf(TEXT("%s has exited the game!"), *playerName)
		);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyGameState.h"
#include "Net/UnrealNetwork.h"
#include "Bounty/PlayerState/BountyPlayerState.h"

void ABountyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABountyGameState, TopScoringPlayers);
}

void ABountyGameState::UpdateTopScore(ABountyPlayerState* _scoringPlayer)
{
	if (0 == TopScoringPlayers.Num())
	{
		TopScoringPlayers.Add(_scoringPlayer);
		TopScore = _scoringPlayer->GetScore();
	}
	else if (TopScore == _scoringPlayer->GetScore())
	{
		TopScoringPlayers.AddUnique(_scoringPlayer);
	}
	else if (TopScore < _scoringPlayer->GetScore())
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(_scoringPlayer);
		TopScore = _scoringPlayer->GetScore();
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BountyGameState.generated.h"

class ABountyPlayerState;
/**
 * 
 */
UCLASS()
class BOUNTY_API ABountyGameState : public AGameState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(ABountyPlayerState* _scoringPlayer);
	UPROPERTY(Replicated)
	TArray<ABountyPlayerState*> TopScoringPlayers;
private:
	float TopScore = 0.f;
};

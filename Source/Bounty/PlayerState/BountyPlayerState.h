// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BountyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BOUNTY_API ABountyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
private:
	class ABountyCharacter* Player;
	class ABountyPlayerController* PlayerController;

public:
	virtual void OnRep_Score() override;	// client
	void AddToScore(float _scoreAmount);	// server
};

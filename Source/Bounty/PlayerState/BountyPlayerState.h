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
	UPROPERTY()
	class ABountyCharacter* Player;
	UPROPERTY()
	class ABountyPlayerController* PlayerController;

	UPROPERTY(ReplicatedUsing = OnRep_LifeLoss)
	int32 LifeLoss;

private:

public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual void OnRep_Score() override;	// client
	void AddToScore(float _scoreAmount);	// server

	UFUNCTION()
	virtual void OnRep_LifeLoss();
	void AddToLifeLoss(int32 _lossAmount);
};

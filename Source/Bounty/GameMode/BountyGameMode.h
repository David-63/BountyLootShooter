// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BountyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BOUNTY_API ABountyGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	virtual void PlayerEliminated(class ABountyCharacter* _elimmedCharacter, class ABountyPlayerController* _victimController, ABountyPlayerController* _attackerController);
};

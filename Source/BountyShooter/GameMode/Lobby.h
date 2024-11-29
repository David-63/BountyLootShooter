// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Lobby.generated.h"

/**
 * 
 */
UCLASS()
class BOUNTYSHOOTER_API ALobby : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* _otherPlayer) override;
	virtual void Logout(AController* _exiting) override;
	
};

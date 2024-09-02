// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BountyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BOUNTY_API ABountyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	class ABountyHUD* BountyHUD;

private:
	virtual void BeginPlay() override;

public:
	void SetHUD_Health(float _healthCur, float _healthMax);


};

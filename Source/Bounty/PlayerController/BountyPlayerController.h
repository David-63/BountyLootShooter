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
	UPROPERTY()
	class ABountyHUD* BountyHUD;

private:
	virtual void BeginPlay() override;

public:
	virtual void OnPossess(APawn* _inPawn) override;
	void SetHUD_Health(float _healthCur, float _healthMax);
	void SetHUD_Score(float _score);
	void SetHUD_LifeLoss(int32 _count);

};

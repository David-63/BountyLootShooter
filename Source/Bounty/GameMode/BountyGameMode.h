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
	ABountyGameMode();
	virtual void Tick(float _deltaTime) override;
	virtual void PlayerEliminated(class ABountyCharacter* _elimmedCharacter, class ABountyPlayerController* _victimController, ABountyPlayerController* _attackerController);
	virtual void RequestRespawn(ACharacter* _elimmedCharacter, AController* _elimmedController);
protected:
	virtual void BeginPlay() override;

public:
	float LevelStartingTime = 0.f;
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 5.f;
private:
	float CountdownTime = 0.f; // warmup Ã¼Å©¿ë

protected:
	virtual void OnMatchStateSet() override;

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyGameMode.h"
#include "Bounty/Character/BountyCharacter.h"
#include "Bounty/PlayerController/BountyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Bounty/PlayerState/BountyPlayerState.h"



namespace MatchState
{
	const FName Cooldown = FName("Cooldown"); // Match duratino has been reached. Display winner and begin cooldown timer
}

ABountyGameMode::ABountyGameMode()
{
	bDelayedStart = true;
}
void ABountyGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ABountyGameMode::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = LevelStartingTime + WarmupTime - GetWorld()->GetTimeSeconds();
		if (0.f >= CountdownTime)
		{
			StartMatch();
		}
	}
	if (MatchState == MatchState::InProgress)
	{
		CountdownTime = LevelStartingTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds();
		if (0.f >= CountdownTime)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = LevelStartingTime + WarmupTime + MatchTime + CooldownTime - GetWorld()->GetTimeSeconds();
		if (0.f >= CountdownTime)
		{
			RestartGame();
		}
	}
}

void ABountyGameMode::PlayerEliminated(ABountyCharacter* _elimmedCharacter, ABountyPlayerController* _victimController, ABountyPlayerController* _attackerController)
{
	ABountyPlayerState* attackerState = _attackerController ? Cast<ABountyPlayerState>(_attackerController->PlayerState) : nullptr;
	ABountyPlayerState* victimState = _victimController ? Cast<ABountyPlayerState>(_victimController->PlayerState) : nullptr;

	if (attackerState && attackerState != victimState)
	{
		attackerState->AddToScore(1.f);
	}
	if (victimState)
	{
		victimState->AddToLifeLoss(1);
	}

	if (_elimmedCharacter)
	{
		_elimmedCharacter->Elim();
	}
	
}

void ABountyGameMode::RequestRespawn(ACharacter* _elimmedCharacter, AController* _elimmedController)
{
	if (_elimmedCharacter)
	{
		_elimmedCharacter->Reset();
		_elimmedCharacter->Destroy();
	}
	if (_elimmedController)
	{
		TArray<AActor*> playerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), playerStarts);
		int32 selection = FMath::RandRange(0, playerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(_elimmedController, playerStarts[selection]);
	}
}


void ABountyGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
	
	for (FConstPlayerControllerIterator playerCtrl = GetWorld()->GetPlayerControllerIterator(); playerCtrl; ++playerCtrl)
	{
		ABountyPlayerController* bountyPlayer = Cast<ABountyPlayerController>(*playerCtrl);
		if (bountyPlayer)
		{
			bountyPlayer->OnMatchStateSet(MatchState);
		}
	}
}
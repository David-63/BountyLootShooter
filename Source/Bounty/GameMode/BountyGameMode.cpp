// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyGameMode.h"
#include "Bounty/Character/BountyCharacter.h"
#include "Bounty/PlayerController/BountyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Bounty/PlayerState/BountyPlayerState.h"


ABountyGameMode::ABountyGameMode()
{
	bDelayedStart = true;
}

void ABountyGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStatingTime = GetWorld()->GetTimeSeconds();
}
void ABountyGameMode::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	if (MatchState::WaitingToStart == MatchState)
	{
		// 남은 시간 = 워밍업 제한 시간 + (시작지점 시간 - 현재 시간)
		CountdownTime = WarmupTime + (LevelStatingTime - GetWorld()->GetTimeSeconds());
		if (0.f >= CountdownTime)
		{
			StartMatch();
		}
	}

}

void ABountyGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator ctrl = GetWorld()->GetPlayerControllerIterator(); ctrl; ++ctrl)
	{
		ABountyPlayerController* playerCtrl = Cast<ABountyPlayerController>(*ctrl);
		if (playerCtrl)
		{
			playerCtrl->OnMatchStateSet(MatchState);
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

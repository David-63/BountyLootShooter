// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyPlayerController.h"
#include "Bounty/HUD/BountyHUD.h"
#include "Bounty/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Bounty/Character/BountyCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Bounty/GameMode/BountyGameMode.h"

void ABountyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BountyHUD = Cast<ABountyHUD>(GetHUD());


}

void ABountyPlayerController::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	SetHUDTime();
	CheckTimeSync(_deltaTime);
	
	PollInit();
}

void ABountyPlayerController::OnPossess(APawn* _inPawn)
{
	Super::OnPossess(_inPawn);

	ABountyCharacter* bountyCharacter = Cast<ABountyCharacter>(_inPawn);
	if (bountyCharacter)
	{		
		SetHUD_Health(bountyCharacter->GetHealth_Cur(), bountyCharacter->GetHealth_Max());
	}
}
void ABountyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABountyPlayerController, MatchState);

}

void ABountyPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}

}


void ABountyPlayerController::SetHUD_Health(float _healthCur, float _healthMax)
{
	BountyHUD = nullptr == BountyHUD ? Cast<ABountyHUD>(GetHUD()) : BountyHUD;

	bool isValidHUD =
		BountyHUD && BountyHUD->CharacterOverlay && BountyHUD->CharacterOverlay->HealthBar && BountyHUD->CharacterOverlay->HealthText;

	if (isValidHUD)
	{
		const float healthRatio = _healthCur / _healthMax;
		BountyHUD->CharacterOverlay->HealthBar->SetPercent(healthRatio);

		FString healthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(_healthCur), FMath::CeilToInt(_healthMax));
		BountyHUD->CharacterOverlay->HealthText->SetText(FText::FromString(healthText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealthCur = _healthCur;
		HUDHealthMax = _healthMax;
	}
}

void ABountyPlayerController::SetHUD_Score(float _score)
{
	BountyHUD = nullptr == BountyHUD ? Cast<ABountyHUD>(GetHUD()) : BountyHUD;
	bool isValidHUD =
		BountyHUD && BountyHUD->CharacterOverlay && BountyHUD->CharacterOverlay->ScoreAmount;

	if (isValidHUD)
	{
		FString scoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(_score));
		BountyHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(scoreText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = _score;
	}

}

void ABountyPlayerController::SetHUD_LifeLoss(int32 _count)
{
	BountyHUD = nullptr == BountyHUD ? Cast<ABountyHUD>(GetHUD()) : BountyHUD;
	bool isValidHUD =
		BountyHUD && BountyHUD->CharacterOverlay && BountyHUD->CharacterOverlay->LifeLossAmount;

	if (isValidHUD)
	{
		FString lifeLossText = FString::Printf(TEXT("%d"), _count);
		BountyHUD->CharacterOverlay->LifeLossAmount->SetText(FText::FromString(lifeLossText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDLifeLoss = _count;
	}
}

void ABountyPlayerController::SetHUD_ExtraAmmo(int32 _count)
{
	BountyHUD = nullptr == BountyHUD ? Cast<ABountyHUD>(GetHUD()) : BountyHUD;
	bool isValidHUD =
		BountyHUD && BountyHUD->CharacterOverlay && BountyHUD->CharacterOverlay->ExtraAmmoAmount;

	if (isValidHUD)
	{
		FString extraAmmoText = FString::Printf(TEXT("%d"), _count);
		BountyHUD->CharacterOverlay->ExtraAmmoAmount->SetText(FText::FromString(extraAmmoText));
	}
}

void ABountyPlayerController::SetHUD_CurrentAmmo(int32 _count)
{
	BountyHUD = nullptr == BountyHUD ? Cast<ABountyHUD>(GetHUD()) : BountyHUD;
	bool isValidHUD =
		BountyHUD && BountyHUD->CharacterOverlay && BountyHUD->CharacterOverlay->CurrentAmmoAmount;

	if (isValidHUD)
	{
		FString currentAmmoText = FString::Printf(TEXT("%d"), _count);
		BountyHUD->CharacterOverlay->CurrentAmmoAmount->SetText(FText::FromString(currentAmmoText));
	}
}

void ABountyPlayerController::SetHUD_MatchTimeCount(float _count)
{
	BountyHUD = nullptr == BountyHUD ? Cast<ABountyHUD>(GetHUD()) : BountyHUD;
	bool isValidHUD =
		BountyHUD && BountyHUD->CharacterOverlay && BountyHUD->CharacterOverlay->MatchTimeCount;

	if (isValidHUD)
	{
		int32 minutes = FMath::FloorToInt(_count / 60.f);
		int32 seconds = _count - (minutes * 60);


		FString matchCount = FString::Printf(TEXT("%02d : %02d"), minutes, seconds);
		BountyHUD->CharacterOverlay->MatchTimeCount->SetText(FText::FromString(matchCount));
	}
}



void ABountyPlayerController::SetHUDTime()
{
	uint32 secondsLeft = FMath::CeilToInt(MatchTime - GetServerTime());

	if (secondsLeft != CountdownInt)
	{
		SetHUD_MatchTimeCount(MatchTime - GetServerTime());
	}

	CountdownInt = secondsLeft;
}

void ABountyPlayerController::ServerRequestServerTime_Implementation(float _timeOfClientRequest)
{
	float serverTimeOfRecipt = GetWorld()->GetTimeSeconds();
	ClientReceivedServerTime(_timeOfClientRequest, serverTimeOfRecipt);
}

void ABountyPlayerController::ClientReceivedServerTime_Implementation(float _timeOfClientRequest, float _timeOfServerReceived)
{
	// _timeOfClientRequest 변수는 처음 서버에 요청한 시점의 시간으로 현재 클라이언트의 시간을 빼면, 클라 <-> 서버 간의 왕복하는데 걸린 시간을 알 수 있음
	float roundTripTime = GetWorld()->GetTimeSeconds() - _timeOfClientRequest;
	// 응답받은 서버 시간에 왕복에 걸린시간을 더하면 현재 서버시간을 알 수 있음
	float currentServerTime = _timeOfServerReceived + (roundTripTime * 0.5f);

	ClientBetweenServerDelta = currentServerTime - GetWorld()->GetTimeSeconds();
}

void ABountyPlayerController::CheckTimeSync(float _deltaTime)
{
	TimeSyncRunningTime += _deltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

float ABountyPlayerController::GetServerTime()
{
	// 서버라면 현재 시간을 반환하고
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	// 클라이언트라면 시간 계산을 해서 반환
	else return GetWorld()->GetTimeSeconds() + ClientBetweenServerDelta;
}

void ABountyPlayerController::OnMatchStateSet(FName _state)
{
	MatchState = _state;

	if (MatchState::InProgress == MatchState)
	{
		BountyHUD = nullptr == BountyHUD ? Cast<ABountyHUD>(GetHUD()) : BountyHUD;
		if (BountyHUD)
		{
			BountyHUD->AddCharacterOverlay();
		}
	}
}


void ABountyPlayerController::PollInit()
{
	if (nullptr == CharacterOverlay)
	{
		if (BountyHUD && BountyHUD->CharacterOverlay)
		{
			CharacterOverlay = BountyHUD->CharacterOverlay;
			if (CharacterOverlay)
			{					
				SetHUD_Health(HUDHealthCur, HUDHealthMax);
				SetHUD_Score(HUDScore);
				SetHUD_LifeLoss(HUDLifeLoss);
			}
		}
	}
}

void ABountyPlayerController::OnRep_MatchState()
{
	if (MatchState::InProgress == MatchState)
	{
		BountyHUD = nullptr == BountyHUD ? Cast<ABountyHUD>(GetHUD()) : BountyHUD;
		if (BountyHUD)
		{
			BountyHUD->AddCharacterOverlay();
		}
	}
}
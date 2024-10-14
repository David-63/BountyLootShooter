// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyPlayerController.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Net/UnrealNetwork.h"
#include "Bounty/HUD/BountyHUD.h"
#include "Bounty/HUD/CharacterOverlay.h"
#include "Bounty/HUD/Announcement.h"
#include "Bounty/Character/BountyCharacter.h"
#include "Bounty/GameMode/BountyGameMode.h"

void ABountyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BountyHUD = Cast<ABountyHUD>(GetHUD());

	if (BountyHUD)
	{
		BountyHUD->AddAnnouncement();
	}

}
void ABountyPlayerController::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);

	SetHUDTime();
	CheckTimeSync(_deltaTime);
	PollInit();
}
void ABountyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABountyPlayerController, BountyMatchState);
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
void ABountyPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}


void ABountyPlayerController::PollInit()
{
	if (!CharacterOverlay)
	{
		if (BountyHUD && BountyHUD->CharacterOverlay)
		{
			CharacterOverlay = BountyHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHUD_Health(HUD_HealthCur, HUD_HealthMax);
				SetHUD_Score(HUD_Score);
				SetHUD_LifeLoss(HUD_LifeLoss);
			}
		}
	}
}
void ABountyPlayerController::SetHUDTime()
{
	uint32 secondsLeft = FMath::CeilToInt(MatchTime - GetServerTime());
	// 1초에한번만 호출되게 제한
	if (secondsLeft != CountdownInt)
	{
		SetHUD_MatchCount(MatchTime - GetServerTime());
	}
	CountdownInt = secondsLeft;
}
void ABountyPlayerController::CheckTimeSync(float _deltaTime)
{
	TimeSyncRunningTime += _deltaTime;
	if (IsLocalController() && TimeSyncFrequency < TimeSyncRunningTime)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}
float ABountyPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientSeverDelta;
}
void ABountyPlayerController::ServerRequestServerTime_Implementation(float _timeOfClientRequest)
{
	float serverTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(_timeOfClientRequest, serverTimeOfReceipt);

}
void ABountyPlayerController::ClientReportServerTime_Implementation(float _timeOfClientRequest, float _timeServerReceivedClientRequest)
{
	float roundTripTime = GetWorld()->GetTimeSeconds() - _timeOfClientRequest;
	float currentServerTime = _timeServerReceivedClientRequest + (0.5f * roundTripTime);
	ClientSeverDelta = currentServerTime - GetWorld()->GetTimeSeconds();
}



void ABountyPlayerController::OnMatchStateSet(FName _state)
{
	BountyMatchState = _state;

	/*if (BountyMatchState == MatchState::WaitingToStart)
	{
		BountyHUD = nullptr == BountyHUD ? Cast<ABountyHUD>(GetHUD()) : BountyHUD;
		if (BountyHUD)
		{
			BountyHUD->AddCharacterOverlay();
		}
		
	}*/

	if (BountyMatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}
void ABountyPlayerController::OnRep_MatchState()
{
	if (BountyMatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}
void ABountyPlayerController::HandleMatchHasStarted()
{
	BountyHUD = nullptr == BountyHUD ? Cast<ABountyHUD>(GetHUD()) : BountyHUD;
	if (BountyHUD)
	{
		BountyHUD->AddCharacterOverlay();
		if (BountyHUD->Announcement)
		{
			BountyHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
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
		bIsInitializeOverlay = true;
		HUD_HealthCur = _healthCur;
		HUD_HealthMax = _healthMax;
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
		bIsInitializeOverlay = true;
		HUD_Score = _score;
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
		bIsInitializeOverlay = true;
		HUD_LifeLoss = _count;
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
void ABountyPlayerController::SetHUD_MatchCount(float _time)
{
	BountyHUD = nullptr == BountyHUD ? Cast<ABountyHUD>(GetHUD()) : BountyHUD;
	bool isValidHUD =
		BountyHUD && BountyHUD->CharacterOverlay && BountyHUD->CharacterOverlay->MatchCountText;

	if (isValidHUD)
	{
		int32 minutes = FMath::FloorToInt(_time / 60.f);
		int32 seconds = _time - (minutes * 60);

		FString matchCountText = FString::Printf(TEXT("%02d : %02d"), minutes, seconds);
		BountyHUD->CharacterOverlay->MatchCountText->SetText(FText::FromString(matchCountText));
	}
}


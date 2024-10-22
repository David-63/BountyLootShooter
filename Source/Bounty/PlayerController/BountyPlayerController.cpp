// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyPlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Bounty/Character/BountyCharacter.h"
#include "Bounty/BountyComponents/CombatComponent.h"
#include "Bounty/GameMode/BountyGameMode.h"
#include "Bounty/HUD/BountyHUD.h"
#include "Bounty/HUD/CharacterOverlay.h"
#include "Bounty/HUD/Announcement.h"

void ABountyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BountyHUD = Cast<ABountyHUD>(GetHUD());
	ServerCheckMatchState();
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
	DOREPLIFETIME(ABountyPlayerController, BountyMatchState);

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
void ABountyPlayerController::SetHUD_MatchTimeCount(float _time)
{
	BountyHUD = nullptr == BountyHUD ? Cast<ABountyHUD>(GetHUD()) : BountyHUD;
	bool isValidHUD =
		BountyHUD && BountyHUD->CharacterOverlay && BountyHUD->CharacterOverlay->MatchTimeCount;

	if (isValidHUD)
	{
		if (0.f > _time)
		{
			BountyHUD->CharacterOverlay->MatchTimeCount->SetText(FText());
			return;
		}
		int32 minutes = FMath::FloorToInt(_time / 60.f);
		int32 seconds = _time - (minutes * 60);


		FString matchCount = FString::Printf(TEXT("%02d : %02d"), minutes, seconds);
		BountyHUD->CharacterOverlay->MatchTimeCount->SetText(FText::FromString(matchCount));
	}
}
void ABountyPlayerController::SetHUD_AnnouncementCount(float _time)
{
	BountyHUD = nullptr == BountyHUD ? Cast<ABountyHUD>(GetHUD()) : BountyHUD;
	bool isValidHUD =
		BountyHUD && BountyHUD->Announcement && BountyHUD->Announcement->WarmupTimerText;

	if (isValidHUD)
	{
		if (0.f > _time)
		{
			BountyHUD->Announcement->WarmupTimerText->SetText(FText());
			return;
		}
		int32 minutes = FMath::FloorToInt(_time / 60.f);
		int32 seconds = _time - (minutes * 60);


		FString matchCount = FString::Printf(TEXT("%02d : %02d"), minutes, seconds);
		BountyHUD->Announcement->WarmupTimerText->SetText(FText::FromString(matchCount));
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


void ABountyPlayerController::SetHUDTime()
{
	// ���� �ð� ������ (������ ��� Ŭ���̾�Ʈ�� �ٸ��� ��Ʈ�ѷ��� ���� �ʱ�ȭ�Ǽ� �ð��� �ȸ���)
	bool isAuthority = HasAuthority();
	if (isAuthority)
	{
		BountyGameMode = nullptr == BountyGameMode ? Cast<ABountyGameMode>(UGameplayStatics::GetGameMode(this)) : BountyGameMode;
		if (BountyGameMode)
		{
			LevelStartingTime = BountyGameMode->LevelStartingTime;
		}
	}

	float timeLeft = 0.f;
	if (MatchState::WaitingToStart == BountyMatchState) timeLeft = (LevelStartingTime + WarmupTime) - GetServerTime();
	else if (MatchState::InProgress == BountyMatchState) timeLeft = (LevelStartingTime + WarmupTime + MatchTime) - GetServerTime();
	else if (MatchState::Cooldown == BountyMatchState) timeLeft = (LevelStartingTime + WarmupTime + MatchTime + CooldownTime) - GetServerTime();	
	uint32 secondsLeft = FMath::CeilToInt(timeLeft);

	if (isAuthority)
	{		
		if (BountyGameMode)
		{
			secondsLeft = FMath::CeilToInt(BountyGameMode->LevelStartingTime + BountyGameMode->GetCountdownTime());
		}
	}


	if (secondsLeft != CountdownInt)
	{
		if (MatchState::WaitingToStart == BountyMatchState || MatchState::Cooldown == BountyMatchState)
		{
			SetHUD_AnnouncementCount(timeLeft);
		}
		if (MatchState::InProgress == BountyMatchState)
		{
			SetHUD_MatchTimeCount(timeLeft);
		}
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
	// _timeOfClientRequest ������ ó�� ������ ��û�� ������ �ð����� ���� Ŭ���̾�Ʈ�� �ð��� ����, Ŭ�� <-> ���� ���� �պ��ϴµ� �ɸ� �ð��� �� �� ����
	float roundTripTime = GetWorld()->GetTimeSeconds() - _timeOfClientRequest;
	// ������� ���� �ð��� �պ��� �ɸ��ð��� ���ϸ� ���� �����ð��� �� �� ����
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
	// ������� ���� �ð��� ��ȯ�ϰ�
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	// Ŭ���̾�Ʈ��� �ð� ����� �ؼ� ��ȯ
	else return GetWorld()->GetTimeSeconds() + ClientBetweenServerDelta;
}

void ABountyPlayerController::OnMatchStateSet(FName _state)
{
	BountyMatchState = _state;

	if (MatchState::InProgress == BountyMatchState)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState::Cooldown == BountyMatchState)
	{
		HandleCooldown();
	}
}
void ABountyPlayerController::OnRep_MatchState()
{
	if (MatchState::InProgress == BountyMatchState)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState::Cooldown == BountyMatchState)
	{
		HandleCooldown();
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

void ABountyPlayerController::HandleCooldown()
{
	BountyHUD = nullptr == BountyHUD ? Cast<ABountyHUD>(GetHUD()) : BountyHUD;
	if (BountyHUD)
	{
		BountyHUD->CharacterOverlay->RemoveFromParent();
		bool bHUDValid = BountyHUD->Announcement && BountyHUD->Announcement->AnnouncementText && BountyHUD->Announcement->MatchInfoText;
		if (bHUDValid)
		{
			BountyHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString announcementText("New Match Starts In");
			BountyHUD->Announcement->AnnouncementText->SetText(FText::FromString(announcementText));
			BountyHUD->Announcement->MatchInfoText->SetText(FText());
		}
	}
	
	ABountyCharacter* bountyCharacter = Cast<ABountyCharacter>(GetPawn());
	if (bountyCharacter && bountyCharacter->GetCombat())
	{
		bountyCharacter->bDisableGameplay = true;
		bountyCharacter->GetCombat()->Attack(false);
	}
}


void ABountyPlayerController::ServerCheckMatchState_Implementation()
{
	ABountyGameMode* gameMode = Cast<ABountyGameMode>(UGameplayStatics::GetGameMode(this));
	if (gameMode)
	{
		BountyMatchState = gameMode->GetMatchState();
		LevelStartingTime = gameMode->LevelStartingTime;
		WarmupTime = gameMode->WarmupTime;
		MatchTime = gameMode->MatchTime;
		CooldownTime = gameMode->CooldownTime;
		ClientJoinMidgame(BountyMatchState, LevelStartingTime, WarmupTime, MatchTime, CooldownTime);
	}
}
void ABountyPlayerController::ClientJoinMidgame_Implementation(FName _stateOfMatch, float _levelStartingTime, float _warmupTime, float _matchTime, float _cooldownTime)
{
	BountyMatchState = _stateOfMatch;
	LevelStartingTime = _levelStartingTime;
	WarmupTime = _warmupTime;
	MatchTime = _matchTime;
	CooldownTime = _cooldownTime;
	OnMatchStateSet(BountyMatchState);
	if (BountyHUD && MatchState::WaitingToStart == BountyMatchState)
	{
		BountyHUD->AddAnnouncement();
	}
}

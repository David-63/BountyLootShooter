// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyPlayerController.h"
#include "Bounty/HUD/BountyHUD.h"
#include "Bounty/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Bounty/Character/BountyCharacter.h"

void ABountyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BountyHUD = Cast<ABountyHUD>(GetHUD());


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
}

void ABountyPlayerController::SetHUD_Ammo(int32 _count)
{
	BountyHUD = nullptr == BountyHUD ? Cast<ABountyHUD>(GetHUD()) : BountyHUD;
	bool isValidHUD =
		BountyHUD && BountyHUD->CharacterOverlay && BountyHUD->CharacterOverlay->AmmoAmount;

	if (isValidHUD)
	{
		FString ammoAmountText = FString::Printf(TEXT("%d"), _count);
		BountyHUD->CharacterOverlay->AmmoAmount->SetText(FText::FromString(ammoAmountText));
	}
}

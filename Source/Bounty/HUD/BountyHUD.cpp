// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyHUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"
#include "Announcement.h"

void ABountyHUD::BeginPlay()
{
	Super::BeginPlay();	
}

void ABountyHUD::AddCharacterOverlay()
{
	APlayerController* playerController = GetOwningPlayerController();
	if (playerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(playerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}	
}

void ABountyHUD::AddAnnouncement()
{
	APlayerController* playerController = GetOwningPlayerController();
	if (playerController && AnnouncementClass)
	{
		Announcement = CreateWidget<UAnnouncement>(playerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
}

void ABountyHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!GEngine) return;

	// 뷰포트 중앙지점 전달
	FVector2D viewportSize;
	GEngine->GameViewport->GetViewportSize(viewportSize);

	const FVector2D viewportCencter(viewportSize.X / 2.f, viewportSize.Y / 2.f);
	
	float spreadScaled = CrosshairSpreadMax * CrosshairPackage.SpreadFactor;
	

	if (CrosshairPackage.CrosshairsCenter)
	{
		DrawCrosshair(CrosshairPackage.CrosshairsCenter, viewportCencter, FVector2D(0.f, 0.f), CrosshairPackage.CrosshairsColor);
	}
	if (CrosshairPackage.CrosshairsLeft)
	{
		DrawCrosshair(CrosshairPackage.CrosshairsLeft, viewportCencter, FVector2D(-spreadScaled, 0.f), CrosshairPackage.CrosshairsColor);
	}
	if (CrosshairPackage.CrosshairsRight)
	{
		DrawCrosshair(CrosshairPackage.CrosshairsRight, viewportCencter, FVector2D(spreadScaled, 0.f), CrosshairPackage.CrosshairsColor);
	}
	if (CrosshairPackage.CrosshairsTop)
	{
		DrawCrosshair(CrosshairPackage.CrosshairsTop, viewportCencter, FVector2D(0.f, -spreadScaled), CrosshairPackage.CrosshairsColor);
	}
	if (CrosshairPackage.CrosshairsBottom)
	{
		DrawCrosshair(CrosshairPackage.CrosshairsBottom, viewportCencter, FVector2D(0.f, spreadScaled), CrosshairPackage.CrosshairsColor);
	}

}

void ABountyHUD::DrawCrosshair(UTexture2D* _texture, FVector2D _viewportCenter, FVector2D _spread, FLinearColor _color)
{
	const float textureWidth = _texture->GetSizeX();
	const float textureHeight = _texture->GetSizeY();
	
	// InertiaValue 값은 Combat Tick의 SetHUD 로직에서 Magnitude 값이 적용된 상태로 넘어옴
	const FVector2D textureDrawPoint(
		_viewportCenter.X - (textureWidth / 2.f) + InertiaValue.X + _spread.X
		, _viewportCenter.Y - (textureHeight / 2.f) + InertiaValue.Y + _spread.Y);
	DrawTexture(_texture, textureDrawPoint.X, textureDrawPoint.Y, textureWidth, textureHeight, 0.f, 0.f, 1.f, 1.f, _color);
}




// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyHUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"

void ABountyHUD::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* playerController = GetOwningPlayerController();
	if (playerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(playerController, CharacterOverlayClass);
		
	}
}

void ABountyHUD::AddCharacterOverlay()
{
	CharacterOverlay->AddToViewport();
}

void ABountyHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!GEngine) return;

	// ����Ʈ �߾����� ����
	FVector2D viewportSize;
	GEngine->GameViewport->GetViewportSize(viewportSize);

	const FVector2D viewportCencter(viewportSize.X / 2.f, viewportSize.Y / 2.f);
	
	float spreadScaled = CrosshairSpreadMax * HUDPackage.SpreadFactor;
	

	if (HUDPackage.CrosshairsCenter)
	{
		DrawCrosshair(HUDPackage.CrosshairsCenter, viewportCencter, FVector2D(0.f, 0.f), HUDPackage.CrosshairsColor);
	}
	if (HUDPackage.CrosshairsLeft)
	{
		DrawCrosshair(HUDPackage.CrosshairsLeft, viewportCencter, FVector2D(-spreadScaled, 0.f), HUDPackage.CrosshairsColor);
	}
	if (HUDPackage.CrosshairsRight)
	{
		DrawCrosshair(HUDPackage.CrosshairsRight, viewportCencter, FVector2D(spreadScaled, 0.f), HUDPackage.CrosshairsColor);
	}
	if (HUDPackage.CrosshairsTop)
	{
		DrawCrosshair(HUDPackage.CrosshairsTop, viewportCencter, FVector2D(0.f, -spreadScaled), HUDPackage.CrosshairsColor);
	}
	if (HUDPackage.CrosshairsBottom)
	{
		DrawCrosshair(HUDPackage.CrosshairsBottom, viewportCencter, FVector2D(0.f, spreadScaled), HUDPackage.CrosshairsColor);
	}

}

void ABountyHUD::DrawCrosshair(UTexture2D* _texture, FVector2D _viewportCenter, FVector2D _spread, FLinearColor _color)
{
	const float textureWidth = _texture->GetSizeX();
	const float textureHeight = _texture->GetSizeY();
	
	// InertiaValue ���� Combat Tick�� SetHUD �������� Magnitude ���� ����� ���·� �Ѿ��
	const FVector2D textureDrawPoint(
		_viewportCenter.X - (textureWidth / 2.f) + InertiaValue.X + _spread.X
		, _viewportCenter.Y - (textureHeight / 2.f) + InertiaValue.Y + _spread.Y);
	DrawTexture(_texture, textureDrawPoint.X, textureDrawPoint.Y, textureWidth, textureHeight, 0.f, 0.f, 1.f, 1.f, _color);
}




// Fill out your copyright notice in the Description page of Project Settings.


#include "BountyHUD.h"



void ABountyHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!GEngine) return;

	FVector2D viewportSize;

	GEngine->GameViewport->GetViewportSize(viewportSize);
	const FVector2D viewportCencter(viewportSize.X / 2.f, viewportSize.Y / 2.f);

	if (HUDPackage.CrosshairsCenter)
	{
		DrawCrosshair(HUDPackage.CrosshairsCenter, viewportCencter);
	}
	if (HUDPackage.CrosshairsLeft)
	{
		DrawCrosshair(HUDPackage.CrosshairsLeft, viewportCencter);
	}
	if (HUDPackage.CrosshairsRight)
	{
		DrawCrosshair(HUDPackage.CrosshairsRight, viewportCencter);
	}
	if (HUDPackage.CrosshairsTop)
	{
		DrawCrosshair(HUDPackage.CrosshairsTop, viewportCencter);
	}
	if (HUDPackage.CrosshairsBottom)
	{
		DrawCrosshair(HUDPackage.CrosshairsBottom, viewportCencter);
	}

}

void ABountyHUD::DrawCrosshair(UTexture2D* _texture, FVector2D _viewportCenter)
{
	const float textureWidth = _texture->GetSizeX();
	const float textureHeight = _texture->GetSizeY();

	const FVector2D textureDrawPoint(_viewportCenter.X - (textureWidth / 2.f), _viewportCenter.Y - (textureHeight / 2.f));
	DrawTexture(_texture, textureDrawPoint.X, textureDrawPoint.Y, textureWidth, textureHeight, 0.f, 0.f, 1.f, 1.f, FLinearColor::White);
}
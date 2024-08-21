// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BountyHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;
	float SpreadFactor;
};

/**
 * 
 */
UCLASS()
class BOUNTY_API ABountyHUD : public AHUD
{
	GENERATED_BODY()

private:
	FHUDPackage HUDPackage;
	FVector2D InertiaValue;

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
	
private:
	void DrawCrosshair(UTexture2D* _texture, FVector2D _viewportCenter, FVector2D _spread);

public:
	virtual void DrawHUD() override;

	FORCEINLINE void SetHUDPackage(const FHUDPackage& _package, const FVector2D& _inertiaValue) { HUDPackage = _package; InertiaValue = _inertiaValue; }
};

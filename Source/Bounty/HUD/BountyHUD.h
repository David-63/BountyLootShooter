// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BountyHUD.generated.h"

USTRUCT(BlueprintType)
struct FCrosshairPackage
{
	GENERATED_BODY()
public:
	UPROPERTY()
	class UTexture2D* CrosshairsCenter;
	UPROPERTY()
	UTexture2D* CrosshairsLeft;
	UPROPERTY()
	UTexture2D* CrosshairsRight;
	UPROPERTY()
	UTexture2D* CrosshairsTop;
	UPROPERTY()
	UTexture2D* CrosshairsBottom;	
	FLinearColor CrosshairsColor;
	float SpreadFactor;
};

/**
 * 
 */
UCLASS()
class BOUNTY_API ABountyHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void DrawHUD() override;


	/*
	* crosshair
	*/
private:
	UPROPERTY()
	FCrosshairPackage CrosshairPackage;
	FVector2D InertiaValue;
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
	void DrawCrosshair(UTexture2D* _texture, FVector2D _viewportCenter, FVector2D _spread, FLinearColor _color);
public:
	FORCEINLINE void SetCrosshairPackage(const FCrosshairPackage& _package, const FVector2D& _inertiaValue) { CrosshairPackage = _package; InertiaValue = _inertiaValue; }



	/*
	* Character hud
	*/
public:
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	void AddCharacterOverlay();

	/*
	* Announcement hud
	*/
	UPROPERTY(EditAnywhere, Category = "Announcement")
	TSubclassOf<class UUserWidget> AnnouncementClass;
	UPROPERTY()
	class UAnnouncement* Announcement;
	void AddAnnouncement();


};

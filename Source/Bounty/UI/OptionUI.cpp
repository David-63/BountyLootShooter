// Fill out your copyright notice in the Description page of Project Settings.


#include "OptionUI.h"
#include "Components/Button.h"
#include "GameFramework/GameUserSettings.h"
#include "Components/TextBlock.h"

void UOptionUI::OptionSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			FInputModeUIOnly inputModeData;
			inputModeData.SetWidgetToFocus(TakeWidget());
			inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			playerController->SetInputMode(inputModeData);
			playerController->SetShowMouseCursor(true);
		}
	}
}

bool UOptionUI::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (WindowPrev)
	{
		WindowPrev->OnClicked.AddDynamic(this, &ThisClass::WindowPrevClicked);
	}
	if (WindowNext)
	{
		WindowNext->OnClicked.AddDynamic(this, &ThisClass::WindowNextClicked);
	}

	if (ResolutionPrev)
	{
		ResolutionPrev->OnClicked.AddDynamic(this, &ThisClass::ResolutionPrevClicked);
	}
	if (ResolutionNext)
	{
		ResolutionNext->OnClicked.AddDynamic(this, &ThisClass::ResolutionNextClicked);
	}

	if (VSyncPrev)
	{
		VSyncPrev->OnClicked.AddDynamic(this, &ThisClass::VSyncPrevClicked);
	}
	if (VSyncNext)
	{
		VSyncNext->OnClicked.AddDynamic(this, &ThisClass::VSyncNextClicked);
	}

	if (GrapicsPrev)
	{
		GrapicsPrev->OnClicked.AddDynamic(this, &ThisClass::GrapicsPrevClicked);
	}
	if (GrapicsNext)
	{
		GrapicsNext->OnClicked.AddDynamic(this, &ThisClass::GrapicsNextClicked);
	}

	if (ApplyBtn)
	{
		ApplyBtn->OnClicked.AddDynamic(this, &ThisClass::ApplyBtnClicked);
	}
	if (QuitBtn)
	{
		QuitBtn->OnClicked.AddDynamic(this, &ThisClass::QuitBtnClicked);
	}

	return true;
}

void UOptionUI::NativeConstruct()
{
	Super::NativeConstruct();

	UGameUserSettings* userSetting = GEngine->GetGameUserSettings();
	userSetting->LoadSettings();
	WindowMode = userSetting->GetFullscreenMode();
	Resolution = userSetting->GetScreenResolution();
	GrapicsIdx = userSetting->GetOverallScalabilityLevel();
	if (1280 == Resolution.X)
	{
		ResolutionIdx = 0;
	}
	else if (1600 == Resolution.X)
	{
		ResolutionIdx = 1;

	}
	else if (1920 == Resolution.X)
	{
		ResolutionIdx = 2;

	}
	else if (2560 == Resolution.X)
	{
		ResolutionIdx = 3;

	}
	else if (3840 == Resolution.X)
	{
		ResolutionIdx = 4;
	}

	FString text;

	switch (WindowMode)
	{
	case EWindowMode::Fullscreen:
		text = FString::Printf(TEXT("Fullscreen"));
		break;
	case EWindowMode::WindowedFullscreen:
		text = FString::Printf(TEXT("WindowedFullscreen"));
		break;
	case EWindowMode::Windowed:
		text = FString::Printf(TEXT("Windowed"));
		break;
	case EWindowMode::NumWindowModes:
		text = FString::Printf(TEXT("NumWindowModes"));
		break;
	default:
		break;
	}
	WindowModeText->SetText(FText::FromString(text));
	text = FString::Printf(TEXT("%d, %d"), Resolution.X, Resolution.Y);
	ResolutionText->SetText(FText::FromString(text));
	text = FString::Printf(TEXT("Off"));
	if (bUseVSync)
	{
		text = FString::Printf(TEXT("On"));
	}
	VSyncText->SetText(FText::FromString(text));
	switch (GrapicsIdx)
	{
	case 0:
		text = FString::Printf(TEXT("Low"));
		break;
	case 1:
		text = FString::Printf(TEXT("Medium"));
		break;
	case 2:
		text = FString::Printf(TEXT("High"));
		break;
	case 3:
		text = FString::Printf(TEXT("Epic"));
		break;
	case 4:
		text = FString::Printf(TEXT("Ultra"));
		break;
	default:
		break;
	}
	GrapicsText->SetText(FText::FromString(text));


	userSetting->SetFullscreenMode(WindowMode);
	userSetting->SetScreenResolution(Resolution);
	userSetting->SetVSyncEnabled(bUseVSync);
	userSetting->SetOverallScalabilityLevel(GrapicsIdx);
	userSetting->ApplySettings(true);
}

void UOptionUI::NativeDestruct()
{
	MenuTearDown();

	Super::NativeDestruct();
}

void UOptionUI::WindowPrevClicked()
{
	TEnumAsByte<EWindowMode::Type> EnumAsByte = WindowMode;
	uint8 EnumToByte = EnumAsByte.GetValue();
	EnumToByte++;
	EnumToByte = FMath::Clamp(EnumToByte, 0, 2);

	WindowMode = static_cast<EWindowMode::Type>(EnumToByte);
	FString text;

	switch (WindowMode)
	{
	case EWindowMode::Fullscreen:
		text = FString::Printf(TEXT("Fullscreen"));
		break;
	case EWindowMode::WindowedFullscreen:
		text = FString::Printf(TEXT("WindowedFullscreen"));
		break;
	case EWindowMode::Windowed:
		text = FString::Printf(TEXT("Windowed"));
		break;
	case EWindowMode::NumWindowModes:
		text = FString::Printf(TEXT("NumWindowModes"));
		break;
	default:
		break;
	}
	WindowModeText->SetText(FText::FromString(text));
}

void UOptionUI::WindowNextClicked()
{
	TEnumAsByte<EWindowMode::Type> EnumAsByte = WindowMode;
	uint8 EnumToByte = EnumAsByte.GetValue();
	EnumToByte--;
	EnumToByte = FMath::Clamp(EnumToByte, 0, 2);

	WindowMode = static_cast<EWindowMode::Type>(EnumToByte);
	FString text;

	switch (WindowMode)
	{
	case EWindowMode::Fullscreen:
		text = FString::Printf(TEXT("Fullscreen"));
		break;
	case EWindowMode::WindowedFullscreen:
		text = FString::Printf(TEXT("WindowedFullscreen"));
		break;
	case EWindowMode::Windowed:
		text = FString::Printf(TEXT("Windowed"));
		break;
	case EWindowMode::NumWindowModes:
		text = FString::Printf(TEXT("NumWindowModes"));
		break;
	default:
		break;
	}
	WindowModeText->SetText(FText::FromString(text));
}

void UOptionUI::ResolutionPrevClicked()
{
	ResolutionIdx--;
	ResolutionIdx = FMath::Clamp(ResolutionIdx, 0, 4);

	switch (ResolutionIdx)
	{
	case 0:
		Resolution = { 1280, 720 };
		break;
	case 1:
		Resolution = { 1600, 900 };
		break;
	case 2:
		Resolution = { 1920, 1080 };
		break;
	case 3:
		Resolution = { 2560, 1440 };
		break;
	case 4:
		Resolution = { 3840, 2160 };
		break;
	default:
		break;
	}

	FString text = FString::Printf(TEXT("%d, %d"), Resolution.X, Resolution.Y);
	ResolutionText->SetText(FText::FromString(text));
}

void UOptionUI::ResolutionNextClicked()
{
	ResolutionIdx++;
	ResolutionIdx = FMath::Clamp(ResolutionIdx, 0, 4);

	switch (ResolutionIdx)
	{
	case 0:
		Resolution = { 1280, 720 };
		break;
	case 1:
		Resolution = { 1600, 900 };
		break;
	case 2:
		Resolution = { 1920, 1080 };
		break;
	case 3:
		Resolution = { 2560, 1440 };
		break;
	case 4:
		Resolution = { 3840, 2160 };
		break;
	default:
		break;
	}

	FString text = FString::Printf(TEXT("%d, %d"), Resolution.X, Resolution.Y);
	ResolutionText->SetText(FText::FromString(text));
}

void UOptionUI::VSyncPrevClicked()
{
	bUseVSync = false;
	FString text = FString::Printf(TEXT("Off"));
	VSyncText->SetText(FText::FromString(text));
}

void UOptionUI::VSyncNextClicked()
{
	bUseVSync = true;
	FString text = FString::Printf(TEXT("On"));
	VSyncText->SetText(FText::FromString(text));
}

void UOptionUI::GrapicsPrevClicked()
{
	GrapicsIdx--;
	GrapicsIdx = FMath::Clamp(GrapicsIdx, 0, 3);
	FString text;
	switch (GrapicsIdx)
	{
	case 0:
		text = FString::Printf(TEXT("Low"));
		break;
	case 1:
		text = FString::Printf(TEXT("Medium"));
		break;
	case 2:
		text = FString::Printf(TEXT("High"));
		break;
	case 3:
		text = FString::Printf(TEXT("Epic"));
		break;
	case 4:
		text = FString::Printf(TEXT("Ultra"));
		break;
	default:
		break;
	}
	GrapicsText->SetText(FText::FromString(text));
}

void UOptionUI::GrapicsNextClicked()
{
	GrapicsIdx++;
	GrapicsIdx = FMath::Clamp(GrapicsIdx, 0, 3);
	FString text;
	switch (GrapicsIdx)
	{
	case 0:
		text = FString::Printf(TEXT("Low"));
		break;
	case 1:
		text = FString::Printf(TEXT("Medium"));
		break;
	case 2:
		text = FString::Printf(TEXT("High"));
		break;
	case 3:
		text = FString::Printf(TEXT("Epic"));
		break;
	case 4:
		text = FString::Printf(TEXT("Ultra"));
		break;
	default:
		break;
	}
	GrapicsText->SetText(FText::FromString(text));
}

void UOptionUI::ApplyBtnClicked()
{
	UGameUserSettings* userSetting = GEngine->GetGameUserSettings();
	userSetting->SetFullscreenMode(WindowMode);
	userSetting->SetScreenResolution(Resolution);
	userSetting->SetVSyncEnabled(bUseVSync);
	userSetting->SetOverallScalabilityLevel(GrapicsIdx);
	userSetting->ApplySettings(true);
}

void UOptionUI::QuitBtnClicked()
{
	NativeDestruct();
}

void UOptionUI::MenuTearDown()
{
	RemoveFromParent();
	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			FInputModeGameOnly inputModeGameOnly;
			playerController->SetInputMode(inputModeGameOnly);
			playerController->SetShowMouseCursor(false);
		}
	}
}

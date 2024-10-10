// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionUI.generated.h"

class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class BOUNTY_API UOptionUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void OptionSetup();

protected:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;


private:
	UPROPERTY(meta = (BindWidget))
	UButton* WindowPrev;
	UPROPERTY(meta = (BindWidget))
	UButton* WindowNext;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WindowModeText;
	EWindowMode::Type WindowMode;
	UFUNCTION()
	void WindowPrevClicked();
	UFUNCTION()
	void WindowNextClicked();



	UPROPERTY(meta = (BindWidget))
	UButton* ResolutionPrev;
	UPROPERTY(meta = (BindWidget))
	UButton* ResolutionNext;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResolutionText;
	int32 ResolutionIdx;
	FIntPoint Resolution;
	UFUNCTION()
	void ResolutionPrevClicked();
	UFUNCTION()
	void ResolutionNextClicked();



	UPROPERTY(meta = (BindWidget))
	UButton* VSyncPrev;
	UPROPERTY(meta = (BindWidget))
	UButton* VSyncNext;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* VSyncText;
	bool bUseVSync;
	UFUNCTION()
	void VSyncPrevClicked();
	UFUNCTION()
	void VSyncNextClicked();



	UPROPERTY(meta = (BindWidget))
	UButton* GrapicsPrev;
	UPROPERTY(meta = (BindWidget))
	UButton* GrapicsNext;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GrapicsText;
	int32 GrapicsIdx;

	UFUNCTION()
	void GrapicsPrevClicked();
	UFUNCTION()
	void GrapicsNextClicked();



	UPROPERTY(meta = (BindWidget))
	UButton* ApplyBtn;
	UPROPERTY(meta = (BindWidget))
	UButton* QuitBtn;
	UFUNCTION()
	void ApplyBtnClicked();
	UFUNCTION()
	void QuitBtnClicked();

	void MenuTearDown();

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

/**
 * 
 */
UCLASS()
class BOUNTYSHOOTER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;

	void SetDisplayText(FString _text);
	UFUNCTION(BlueprintCallable)
	void ShowPlayerName(APawn* _pawn);

protected:
	virtual void NativeDestruct() override;

};

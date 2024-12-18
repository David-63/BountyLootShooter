// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "ItemMeshComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Items), meta = (BlueprintSpawnableComponent))
class BOUNTYSHOOTER_API UItemMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	UItemMeshComponent();


protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION()
	void ItemCollisionDisable();

	// ���� ������ ����ٸ� �浹 ���� ä���� �����ϵ���
	UFUNCTION()
	void ItemCollisionEnable();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CartridgeCase.generated.h"

UCLASS()
class BOUNTYSHOOTER_API ACartridgeCase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACartridgeCase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CartridgeCaseMesh;

	UPROPERTY(EditAnywhere)
	float ShellEjectionImpulse = 5.5f;
	UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound;

	FTimerHandle* DestroyTimerHandle;

	FVector OwnerVelocity;

protected:
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit);

public:
	void CasingImpulse();

};

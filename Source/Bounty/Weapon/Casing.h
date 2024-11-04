// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class BOUNTY_API ACasing : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CasingMesh;

	UPROPERTY(EditAnywhere)
	float ShellEjectionImpulse = 3.5f;
	UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound;

	FTimerHandle* DestroyTimerHandle;

	FVector OwnerVelocity;

protected:
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit);

public:
	void CasingImpulse();
	FORCEINLINE void SetOwnerVelocity(const FVector _velocity) { OwnerVelocity = _velocity; }

public:	
	ACasing();

protected:
	virtual void BeginPlay() override;


};

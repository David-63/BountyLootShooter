// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Bounty/HUD/BountyHUD.h"
#include "CombatComponent.generated.h"


#define TRACE_LENGTH 10000.f

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOUNTY_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	friend class ABountyCharacter;

private:
	class ABountyCharacter* Character;
	class ABountyPlayerController* PlayerController;
	class ABountyHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquipWeapon)
	class ABaseWeapon* EquippedWeapon;
	UPROPERTY(Replicated)
	bool bIsADS;

	UPROPERTY(EditAnywhere, Category = "Combat Movement")
	float BaseMoveSpeed = 550.f;
	UPROPERTY(EditAnywhere, Category = "Combat Movement")
	float ADSMoveSpeed = 250.f;

	bool bIsAttackDown;

	// HUD and crosshair
	FHUDPackage HUDPackage;


	UPROPERTY(EditAnywhere, Category = "CrossHair")
	float BaseSpread = 0.5f;
	UPROPERTY(EditAnywhere, Category = "CrossHair")
	float InertiaMagnitude = 20.f;

	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairAttackingFactor;

	UPROPERTY(EditAnywhere, Category = "CrossHair")
	float SpreadCorrection = 0.45f;				// ���� ����

	UPROPERTY(EditAnywhere, Category = "CrossHair")
	float SpreadMOA = 1.8f;					// Minute Of Angle (��� �г�Ƽ ���� �����)
	UPROPERTY(EditAnywhere, Category = "CrossHair")
	float RecoveryMOA = 10.f;				// �ݵ� ȸ���ӵ�

	// for fabrik
	FVector HitTarget;
		
	/*
	*  ADS and FOV
	*/

	// Set to the camera's base FOV in BeginPlay
	float DefaultFOV;
	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = "CrossHair")
	float ZoomedFOV = 45.f;
	UPROPERTY(EditAnywhere, Category = "CrossHair")
	float ZoomInterpSpeed = 30.f;


	/*
	* Auto Fire
	*/

	FTimerHandle FireTimer;
	UPROPERTY(EditAnywhere, Category = Combat)
	float FireDelay = 0.08f;
	bool bIsAutoAttack = true;
	bool bCanAttack = true;

private:

	void InterpFov(float _deltaTime);
	void StartFireTimer();
	void FireTimerFinished();


protected:
	void SetADS(bool _bIsADS);
	UFUNCTION(Server, Reliable)
	void ServerSetADS(bool _bIsADS);
	UFUNCTION()
	void OnRep_EquipWeapon();

	void Attack(bool _presseed);

	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerAttack(const FVector_NetQuantize& _traceHitTarget);

	// NetMulticast �ɼ����� �������� ȣ��� ��� Ŭ���̾�Ʈ�� �����ϰ� �۵���
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttack(const FVector_NetQuantize& _traceHitTarget);

	void TraceUnderCrosshairs(FHitResult& _traceHitResult);

	void SetHUDCrosshairs(float _deltaTime);

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	// Replaicate ���� ����
	void EquipWeapon(ABaseWeapon* _weaponToEquip);

public:	
	UCombatComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};

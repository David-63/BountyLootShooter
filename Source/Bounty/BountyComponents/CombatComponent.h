// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Bounty/HUD/BountyHUD.h"
#include "Bounty/Weapon/WeaponTypes.h"
#include "Bounty/BountyType/CombatState.h"
#include "CombatComponent.generated.h"


#define TRACE_LENGTH 10000.f


// #���������ڿ� ���� �Լ��� �����ϴ°� �ƴ϶� ��ɺ� �������� �з���

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOUNTY_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	friend class ABountyCharacter;

private:
	UPROPERTY()
	class ABountyCharacter* Character;
	UPROPERTY()
	class ABountyPlayerController* PlayerController;
	UPROPERTY()
	class ABountyHUD* HUD;


	/*
	* Equipped weapon
	*/
private:
	UPROPERTY(ReplicatedUsing = OnRep_EquipWeapon)
	class ABaseWeapon* EquippedWeapon;
protected:
	UFUNCTION()
	void OnRep_EquipWeapon();
public:
	void EquipWeapon(ABaseWeapon* _weaponToEquip);



	/*
	*  HUD and crosshair
	*/
private:
	FHUDPackage HUDPackage;
	UPROPERTY(EditAnywhere, Category = "CrossHair")
	float BaseSpread = 0.5f;
	UPROPERTY(EditAnywhere, Category = "CrossHair")
	float InertiaMagnitude = 20.f;

	float CrosshairInAirFactor;		// �ݵ� ���
	float CrosshairAimFactor;		// �ݵ� ���
	float CrosshairAttackingFactor;	// �ݵ� ���

	UPROPERTY(EditAnywhere, Category = "CrossHair")
	float SpreadCorrection = 0.45f;				// ���� ����

	UPROPERTY(EditAnywhere, Category = "CrossHair")
	float SpreadMOA = 1.8f;					// Minute Of Angle (��� �г�Ƽ ���� �����)
	UPROPERTY(EditAnywhere, Category = "CrossHair")
	float RecoveryMOA = 10.f;				// �ݵ� ȸ���ӵ�

protected:
	void SetHUDCrosshairs(float _deltaTime);
	void TraceUnderCrosshairs(FHitResult& _traceHitResult);


	// for fabrik
private:
	FVector HitTarget;


		
	/*
	*  ADS and FOV
	*/

private:
	UPROPERTY(Replicated)
	bool bIsADS;
	UPROPERTY(EditAnywhere, Category = "Combat Movement")
	float ADSMoveSpeed = 250.f;
	UPROPERTY(EditAnywhere, Category = "Combat Movement")
	float BaseMoveSpeed = 550.f;
	// Set to the camera's base FOV in BeginPlay
	float DefaultFOV;
	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = "CrossHair")
	float ZoomedFOV = 45.f;
	UPROPERTY(EditAnywhere, Category = "CrossHair")
	float ZoomInterpSpeed = 30.f;

	void InterpFov(float _deltaTime);	// aim ���¿� ���� fov �������ִ� �Լ�
protected:
	void SetADS(bool _bIsADS);
	UFUNCTION(Server, Reliable)
	void ServerSetADS(bool _bIsADS);


	/*
	* Auto Fire
	*/

private:
	FTimerHandle FireTimer;

	bool bCanAttack = true;
	bool bIsAttackDown;

	// �� ���ⱺ�� ź�� �� (źâ ����)
	TMap<EWeaponType, int32> ExtraAmmoMap;
	UPROPERTY(ReplicatedUsing = OnRep_ExtraAmmo)
	int32 ExtraAmmo;

	UPROPERTY(EditAnywhere)
	int32 StartingAmmo = 30;

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;


private:
	void InitializeExtraAmmo();
	void StartFireTimer();
	void FireTimerFinished();
	bool CanFire();
	UFUNCTION()
	void OnRep_ExtraAmmo();
	UFUNCTION()
	void OnRep_CombatState();
	void UpdateAmmoValue();

protected:
	void Attack(bool _presseed); // input
	void Fire();				// do attack
	UFUNCTION(Server, Reliable)
	void ServerAttack(const FVector_NetQuantize& _traceHitTarget);	
	UFUNCTION(NetMulticast, Reliable)	// NetMulticast �ɼ����� �������� ȣ��� ��� Ŭ���̾�Ʈ�� �����ϰ� �۵���
	void MulticastAttack(const FVector_NetQuantize& _traceHitTarget);

	UFUNCTION(Server, Reliable)
	void ServerWeaponReload();

	void HandleReload();
	int32 AmountToReload();


public:
	void WeaponReload();
	UFUNCTION(BlueprintCallable)
	void WeaponReloadFinish();



public:	
	UCombatComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	// Replaicate ���� ����

		
};

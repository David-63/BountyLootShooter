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
	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;
	UFUNCTION()
	void OnRep_CombatState();
protected:
	void DropEquippedWeapon();
	void AttachActorToHand(AActor* _actorToAttach, FName _socket);
	void UpdateExtraAmmo();
	void PlayEquipWeaponSound();
	void ReloadEmptyWeapon();
	UFUNCTION()
	void OnRep_EquipWeapon();
public:
	void EquipWeapon(ABaseWeapon* _weaponToEquip);








	/*
	*  HUD and crosshair
	*/
private:
	FCrosshairPackage CrosshairPackage;
	UPROPERTY(EditAnywhere, Category = "CrossHair Factor")
	float BaseSpread = 0.5f;
	UPROPERTY(EditAnywhere, Category = "CrossHair Factor")
	float InertiaMagnitude = 20.f;

	float CrosshairInAirFactor;		// �ݵ� ���
	float CrosshairAimFactor;		// �ݵ� ���
	float CrosshairAttackingFactor;	// �ݵ� ���

	UPROPERTY(EditAnywhere, Category = "CrossHair Factor")
	float SpreadCorrection = 0.45f;				// ���� ����

	UPROPERTY(EditAnywhere, Category = "CrossHair Factor")
	float SpreadMOA = 1.8f;					// Minute Of Angle (��� �г�Ƽ ���� �����)
	UPROPERTY(EditAnywhere, Category = "CrossHair Factor")
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

	UPROPERTY(EditAnywhere, Category = "CrossHair Factor")
	float ZoomedFOV = 45.f;
	UPROPERTY(EditAnywhere, Category = "CrossHair Factor")
	float ZoomInterpSpeed = 30.f;

	void InterpFov(float _deltaTime);	// aim ���¿� ���� fov �������ִ� �Լ�
protected:
	void SetADS(bool _bIsADS);
	UFUNCTION(Server, Reliable)
	void ServerSetADS(bool _bIsADS);



	/*
	* Attack
	*/
private:
	FTimerHandle FireTimer;

	bool bCanAttack = true;
	bool bIsAttackDown;

	void StartFireTimer();
	void FireTimerFinished();
	bool CanFire();

	

protected:
	void Attack();				// do attack
	UFUNCTION(Server, Reliable)
	void ServerAttack(const FVector_NetQuantize& _traceHitTarget);
	UFUNCTION(NetMulticast, Reliable)	// NetMulticast �ɼ����� �������� ȣ��� ��� Ŭ���̾�Ʈ�� �����ϰ� �۵���
	void MulticastAttack(const FVector_NetQuantize& _traceHitTarget);

public:
	void InputAttack(bool _presseed); // input




	/*
	* Reload
	*/
private:
	TMap<EWeaponType, int32> ExtraAmmoMap;

	UPROPERTY(EditAnywhere, Category = "Extra Ammo")
	int32 StartingAmmoAR = 80;
	UPROPERTY(EditAnywhere, Category = "Extra Ammo")
	int32 StartingAmmoRocket = 12;
	UPROPERTY(EditAnywhere, Category = "Extra Ammo")
	int32 StartingAmmoPistol = 30;
	UPROPERTY(EditAnywhere, Category = "Extra Ammo")
	int32 StartingAmmoSMG = 100;
	UPROPERTY(EditAnywhere, Category = "Extra Ammo")
	int32 StartingAmmoScatterGun = 21;
	UPROPERTY(EditAnywhere, Category = "Extra Ammo")
	int32 StartingAmmoDMR = 18;
	UPROPERTY(EditAnywhere, Category = "Extra Ammo")
	int32 StartingAmmoGrenade = 24;

	void InitializeExtraAmmo();

	UPROPERTY(ReplicatedUsing = OnRep_ExtraAmmo)
	int32 ExtraAmmo;
	UFUNCTION()
	void OnRep_ExtraAmmo();

	void UpdateMagazineAmmo();
	void UpdateSingleRoundAmmo();

protected:

	UFUNCTION(Server, Reliable)
	void ServerWeaponReload();

	void HandleReload();
	int32 AmountToReload();

public:
	void WeaponReload();
	UFUNCTION(BlueprintCallable)
	void WeaponReloadFinish();
	UFUNCTION(BlueprintCallable)
	void ReloadSingleRound();
	void JumpToShotGunEnd();

	/*
	* throw grenade
	*/
protected:
	void ThrowGrenade();
	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();
	UPROPERTY(EditAnywhere, Category = "Combat Addon")
	TSubclassOf<class AProjectile> GrenadeClass;
public:
	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();
	UFUNCTION(BlueprintCallable)
	void LaunchGrenade();
	UFUNCTION(Server, Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize& _target);
private:
	void ShowAttachedGrenade(bool _show);

	UPROPERTY(EditAnywhere, Category = "Grenade Count")
	int32 GrenadeMax = 3;
	UPROPERTY(ReplicatedUsing = OnRep_Grenades)
	int32 GrenadeCur = 3;
	UFUNCTION()
	void OnRep_Grenades();
	void UpdateHUDGrenades();
public:
	FORCEINLINE int32 GetGrenadeCount() { return GrenadeCur; }



public:	
	UCombatComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	// Replaicate ���� ����

		
};

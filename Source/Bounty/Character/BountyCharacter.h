// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "Bounty/Interfaces/CrosshairInteractor.h"
#include "Bounty/BountyType/TurningInPlace.h"
#include "Bounty/BountyType/CombatState.h"
#include "BountyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class BOUNTY_API ABountyCharacter : public ACharacter, public ICrosshairInteractor
{
	GENERATED_BODY()

private:
	// Camera
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Addon", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* TpsSpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Addon", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* TpsCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Addon", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* FpsSpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Addon", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FpsCamera;

	FTransform TpsRelativeTransform;
	FTransform FpsRelativeTransform;
	bool bInterpTransition = false;
	bool bIsTransitionComplete = true;
public:
	FORCEINLINE USpringArmComponent* GetTpsSpringArm() const { return TpsSpringArm; }
	FORCEINLINE UCameraComponent* GetTpsCamera() const { return TpsCamera; }
	FORCEINLINE USpringArmComponent* GetFpsSpringArm() const { return FpsSpringArm; }
	FORCEINLINE UCameraComponent* GetFpsCamera() const { return FpsCamera; }
	FORCEINLINE FTransform GetTpsRelativeTransform() const { return TpsRelativeTransform; }
	FORCEINLINE FTransform GetFpsRelativeTransform() const { return FpsRelativeTransform; }
	FORCEINLINE bool IsInterpTransition() { return bInterpTransition; }
	FORCEINLINE void SetInterpTransition(bool _excute) { bInterpTransition = _excute; }

	// UI
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Addon", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	/*
	* Combat & Weapon
	*/
private:
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon) // ReplicatedUsing 프로퍼티로 RepNotify함수를 연결하면, 복사되는 대상에게만 호출되는 함수가 연결된다
	class ABaseWeapon* OverlappingWeapon;	// 변경사항이 생길 경우에만, Replicated 전달
	UFUNCTION()
	void OnRep_OverlappingWeapon(ABaseWeapon* _lastWeapon); // 변동사항으로 인해 레플리케이트 변수가 null이 된 경우, null이 되기 전 값을 인자로 임시저장된 값을 받을 수 있음


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;			// 컴포넌트는 자체적으로 Replicate 설정하는 기능이 있음, 생성자에서 설정

	UPROPERTY(EditAnywhere, Category = "Character Addon")
	class UAnimMontage* FireArmMontage;
	UPROPERTY(EditAnywhere, Category = "Character Addon")
	class UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, Category = "Character Addon")
	class UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere, Category = "Character Addon")
	class UAnimMontage* ThrowMontage;
private:

	UFUNCTION(Server, Reliable)	// RPC 함수중에 Reliable 옵션은 코스트를 많이 먹기 때문에 중요한 경우가 아니면 되도록 사용하지 않는걸 권장
	void ServerInputEquip();

protected:
	UFUNCTION()
	void ReceiveDamage(AActor* _damagedActor, float _damage, const UDamageType* _damageType, class AController* _instegatorController, AActor* _damageCauser); // 서버에서 호출됨

	void PlayHitReactMontage();

public:
	void PlayFireMontage(bool _bADS);
	void PlayReloadMontage();
	void PlayAmmoInsertion();
	void PlayChamberingRound();
	void SetOverlappingWeapon(ABaseWeapon* _weapon);
	void PlayThrowMontage();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool _bShowScope);


	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }

	/*
	* for input controll
	*/

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* IMC_Character;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Move;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Look;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_JumpNDodge;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Equip;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Crouch;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_ADS;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Fire;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Sprint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Reload;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Throw;
protected:
	void InputMove(const FInputActionValue& Value);
	void InputLook(const FInputActionValue& Value);
	void InputEquip(const FInputActionValue& Value);
	void InputCrouch();
	void InputADS();
	void InputFireDown(const FInputActionValue& Value);
	void InputFireRelease(const FInputActionValue& Value);
	virtual void Jump() override;
	void InputThrow();
	void InputChamberingRound();
	void InputAmmoInsertion();



	/*
	* tps animation value
	*/
private:
	float AO_Yaw;
	float AO_Pitch;
	float InterpAO_Yaw;
	FRotator StartingAimRotation;
	ETurningInPlace TurningInPlace;
	UPROPERTY(EditAnywhere, Category = "Camera option")
	float CameraThreshold = 200.f;

	bool bIsRotateRootBone;

	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TurnThreshold = 0.5f;
	float TimeSinceLastMovementReplication;



private:
	void ADS_Offset(float _deltaTime);
	void CalculateAO_Pitch();
	void TurnInPlace(float _deltaTime);
	void SimProxiesTurn();
	void HideCharacterMesh();
	float CalculateSpeed() const;

protected:
	void RotateInPlace(float DeltaTime);

public:
	virtual void OnRep_ReplicatedMovement() override;

	FVector GetHitTarget() const;

	ABaseWeapon* GetEquippedWeapon() const;
	bool IsUsingGamepad() const;
	bool IsWeaponEquipped() const;
	bool IsADS() const;
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bIsRotateRootBone; }

	/*
	* hud
	*/
private:
	UPROPERTY()
	class ABountyPlayerController* BountyPlayerController;
	UPROPERTY()
	class ABountyPlayerState* BountyPlayerState;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float Health_Max = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health_Cur = 100.f;

	FVector2D InertiaValue;		// for crosshair

private:
	UFUNCTION()
	void OnRep_Health(); // 클라이언트에서 호출됨

protected:
	void PollInit();	// Poll for any relelvant classes and initialize our HUD
	void UpdateHUD_Health();

public:
	FORCEINLINE float GetHealth_Max() const { return Health_Max; }
	FORCEINLINE float GetHealth_Cur() const { return Health_Cur; }
	FORCEINLINE FVector2D GetInertiaValue() const { return InertiaValue; }







	/*
	* elim
	*/
private:


	bool bIsElimmed = false;
	FTimerHandle ElimTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Elim option")
	float ElimDelay = 3.f;

	// Dissolve Effect
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere, Category = "Character Addon")
	UCurveFloat* DissolveCurve;

	// dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category = "Character Addon")
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstanceA;
	UPROPERTY(VisibleAnywhere, Category = "Character Addon")
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstanceB;

	// material instance set on the blueprint, used with the dynamic material instnace
	UPROPERTY(EditAnywhere, Category = "Character Addon")
	UMaterialInstance* DissolveMaterialInstanceA;
	UPROPERTY(EditAnywhere, Category = "Character Addon")
	UMaterialInstance* DissolveMaterialInstanceB;

	UPROPERTY(EditAnywhere, Category = "Character Addon")
	class UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = "Character Addon")
	UParticleSystem* ElimBotEffect;
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;
	UPROPERTY(EditAnywhere, Category = "Character Addon")
	class USoundCue* ElimBotSound;

private:

	void ElimTimerFinished();
	UFUNCTION()
	void UpdateDissolveMaterial(float _dissolveValue);
	void StartDissolve();

protected:

public:
	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	void PlayElimMontage();

	FORCEINLINE bool IsElimmed() const { return bIsElimmed; }
	bool bDisableGameplay = false;

	FORCEINLINE bool IsDisableGameplay() const { return bDisableGameplay; }




	/*
	* Grenade
	*/

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* AttachedGrenade;


public:
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }

	// 공용 함수
public:
	ABountyCharacter();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	// Replaicate 변수 초기화

	virtual void Destroyed() override;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Bounty/BountyType/TurningInPlace.h"
#include "Bounty/Interfaces/CrosshairInteractor.h"
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	

	// UI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	// Combat
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon) // ReplicatedUsing ������Ƽ�� RepNotify�Լ��� �����ϸ�, ����Ǵ� ��󿡰Ը� ȣ��Ǵ� �Լ��� ����ȴ�
	class ABaseWeapon* OverlappingWeapon;	// ��������� ���� ��쿡��, Replicated ����
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;			// ������Ʈ�� ��ü������ Replicate �����ϴ� ����� ����, �����ڿ��� ����

	// inputs
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
	UInputAction* IA_ReloadNSwap;

	// visual info
	float AO_Yaw;
	float AO_Pitch;
	float InterpAO_Yaw;
	FRotator StartingAimRotation;
	ETurningInPlace TurningInPlace;
	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;
	bool bIsRotateRootBone;

	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TurnThreshold = 0.5f;
	float TimeSinceLastMovementReplication;


	UPROPERTY(EditAnywhere, Category = "Montage")
	class UAnimMontage* FireArmMontage;
	UPROPERTY(EditAnywhere, Category = "Montage")
	class UAnimMontage* HitReactMontage;

	// CrossHair Inertia
	FVector2D InertiaValue;

	/*
	* player Health
	*/

	class ABountyPlayerController* BountyPlayerController;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float Health_Max = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health_Cur = 100.f;



private:
	UFUNCTION()
	void OnRep_OverlappingWeapon(ABaseWeapon* _lastWeapon); // ������������ ���� ���ø�����Ʈ ������ null�� �� ���, null�� �Ǳ� �� ���� ���ڷ� �ӽ������ ���� ���� �� ����
	UFUNCTION()
	void OnRep_Health();									// Ŭ���̾�Ʈ���� ȣ���

	UFUNCTION(Server, Reliable)	// RPC �Լ��߿� Reliable �ɼ��� �ڽ�Ʈ�� ���� �Ա� ������ �߿��� ��찡 �ƴϸ� �ǵ��� ������� �ʴ°� ����
	void ServerInputEquip();

	void ADS_Offset(float _deltaTime);
	void CalculateAO_Pitch();
	void TurnInPlace(float _deltaTime);
	void SimProxiesTurn();
	void HideCharacterMesh();
	float CalculateSpeed() const;

protected:
	UFUNCTION()
	void ReceiveDamage(AActor* _damagedActor, float _damage, const UDamageType* _damageType, class AController* _instegatorController, AActor* _damageCauser); // �������� ȣ���

	void PlayHitReactMontage();
	void UpdateHUD_Health();



	// input controll
	void InputMove(const FInputActionValue& Value);
	void InputLook(const FInputActionValue& Value);
	void InputEquip(const FInputActionValue& Value);
	void InputCrouch();
	void InputADS();
	void InputFireDown(const FInputActionValue& Value);
	void InputFireRelease(const FInputActionValue& Value);
	virtual void Jump() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	// Replaicate ���� �ʱ�ȭ
	void SetOverlappingWeapon(ABaseWeapon* _weapon);
	virtual void OnRep_ReplicatedMovement() override;



	void PlayFireArmMontage(bool _bADS);



	FVector GetHitTarget() const;
	ABaseWeapon* GetEquippedWeapon() const;
	bool IsUsingGamepad() const;
	bool IsWeaponEquipped() const;
	bool IsADS() const;
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE FVector2D GetInertiaValue() const { return InertiaValue; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bIsRotateRootBone; }


	// default function
public:
	ABountyCharacter();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Bounty/BountyType/TurningInPlace.h"
#include "BountyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class BOUNTY_API ABountyCharacter : public ACharacter
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* IMC_Character;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Move;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Look;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_JumpNDodge;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Equip;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Crouch;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_ADS;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Fire;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Sprint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_ReloadNSwap;

	// visual info
	float AO_Yaw;
	float AO_Pitch;
	float InterpAO_Yaw;
	FRotator StartingAimRotation;
	ETurningInPlace TurningInPlace;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireArmMontage;

	// CrossHair Inertia
	FVector2D InertiaValue;

private:
	UFUNCTION()
	void OnRep_OverlappingWeapon(ABaseWeapon* _lastWeapon); // ������������ ���� ���ø�����Ʈ ������ null�� �� ���, null�� �Ǳ� �� ���� ���ڷ� �ӽ������ ���� ���� �� ����
	UFUNCTION(Server, Reliable)	// RPC �Լ��߿� Reliable �ɼ��� �ڽ�Ʈ�� ���� �Ա� ������ �߿��� ��찡 �ƴϸ� �ǵ��� ������� �ʴ°� ����
	void ServerInputEquip();
	void ADS_Offset(float _deltaTime);
	void TurnInPlace(float _deltaTime);
	
protected:
	// input controll
	void InputMove(const FInputActionValue& Value);
	void InputLook(const FInputActionValue& Value);
	void InputEquip(const FInputActionValue& Value);
	void InputCrouch();
	void InputADS();
	void InputFire();
	virtual void Jump() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	// Replaicate ���� �ʱ�ȭ
	void SetOverlappingWeapon(ABaseWeapon* _weapon);
	bool IsUsingGamepad() const;
	bool IsWeaponEquipped() const;
	bool IsADS() const;

	ABaseWeapon* GetEquippedWeapon() const;
	void PlayFireArmMontage(bool _bADS);
	FVector GetHitTarget() const;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE FVector2D GetInertiaValue() const { return InertiaValue; }

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

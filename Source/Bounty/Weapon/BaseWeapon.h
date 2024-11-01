// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "BaseWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};
class UTexture2D;

UCLASS()
class BOUNTY_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
	/*
	* Actor setting
	*/
private:
	UPROPERTY(VisibleAnywhere, Category = "BaseWeapon Properties")
	USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, Category = "BaseWeapon Properties")
	class USphereComponent* AreaSphere;
	UPROPERTY(VisibleAnywhere, Category = "BaseWeapon Properties")
	class UWidgetComponent* PickupWidget;
	UPROPERTY()
	class ABountyCharacter* BountyOwnerCharacter;
	UPROPERTY()
	class ABountyPlayerController* BountyOwnerController;
	UPROPERTY(VisibleAnywhere, Category = "BaseWeapon Properties", ReplicatedUsing = OnRep_WeaponState)	// 엑터는 replicate를 사용하려면 생성자에서 bReplicates = true 선언해줘야함
		EWeaponState WeaponState;
	UFUNCTION()
	void OnRep_WeaponState();

protected:
	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	void ShowPickupWidget(bool _showWidget);
	void SetWeaponState(EWeaponState _state);
	virtual void OnRep_Owner() override;
	void Dropped();

	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	






	/*
	* weapon function
	*/

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TSubclassOf<class ACasing> CasingClass;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	int32 AmmoMax;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties", ReplicatedUsing = OnRep_Ammo)
	int32 AmmoCur;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EWeaponType WeaponType;
private:
	UFUNCTION()
	void OnRep_Ammo();
	void SpendRound();

public:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float FireDelay = 0.12f;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bUseAutoAttack = true;	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class USoundCue* EquipSound;

public:
	virtual void Fire(const FVector& _hitTarget);
	void SetHUDCurrentAmmo();
	void AddAmmo(int32 _ammoToAdd);
	bool IsAmmoEmpty();
	bool IsAmmoFull();

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetAmmo() const { return AmmoCur; }
	FORCEINLINE int32 GetMagCapacity() const { return AmmoMax; }


	/*
	* weapon aim sight
	*/

public:
	UPROPERTY(EditAnywhere, Category = "Weapon Crosshairs")
	UTexture2D* CrosshairsCenter;
	UPROPERTY(EditAnywhere, Category = "Weapon Crosshairs")
	UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere, Category = "Weapon Crosshairs")
	UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere, Category = "Weapon Crosshairs")
	UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere, Category = "Weapon Crosshairs")
	UTexture2D* CrosshairsBottom;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float ZoomedFOV = 45.f;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float ZoomInterpSpeed = 30.f;
	
public:
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }


	/*
	* custom depth (outline mtrl)
	*/
public:
	void EnableCustomDepth(bool _bEnable);
	


public:	
	ABaseWeapon();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	// Replaicate 변수 초기화
};

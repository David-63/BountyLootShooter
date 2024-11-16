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
class USoundCue;
class AProjectile;
class ACasing;
UCLASS()
class BOUNTY_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	

protected:
	
	UPROPERTY(VisibleAnywhere, Category = "BaseWeapon Addon")
	USkeletalMeshComponent* WeaponMesh = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "BaseWeapon Addon")
	class USphereComponent* PickupArea = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "BaseWeapon Addon")
	class UWidgetComponent* PickupWidget = nullptr;
	UPROPERTY()
	class ABountyCharacter* BountyOwnerCharacter = nullptr;
	UPROPERTY()
	class ABountyPlayerController* BountyOwnerController = nullptr;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_WeaponState)	// 엑터는 replicate를 사용하려면 생성자에서 bReplicates = true 선언해줘야함
	EWeaponState WeaponState;
	UFUNCTION()
	void OnRep_WeaponState();
	UPROPERTY(EditAnywhere, Category = "BaseWeapon Properties")
	EWeaponType WeaponType;
	const USkeletalMeshSocket* MuzzleFlashSocket = nullptr;
	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	void ShowPickupWidget(bool _showWidget);
	void SetWeaponState(EWeaponState _state);
	virtual void OnRep_Owner() override;
	void Dropped();

	FORCEINLINE USphereComponent* GetAreaSphere() const { return PickupArea; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE virtual USoundCue* GetEquipSound() const { return nullptr; };


private:
	UPROPERTY(EditAnywhere, Category = "BaseWeapon Properties")
	int32 AmmoMax = 50;
	UPROPERTY(EditAnywhere, Category = "BaseWeapon Properties", ReplicatedUsing = OnRep_Ammo)
	int32 AmmoCur = 50;
	UPROPERTY(VisibleAnywhere, Category = "BaseWeapon Properties", Replicated)
	bool Chamber = true;
	UFUNCTION()
	void OnRep_Ammo();

public:
	void SpendRound();
	void SetHUDCurrentAmmo();
	void AddAmmo(int32 _ammoToAdd);
	bool IsAmmoEmpty();
	bool IsAmmoFull();
	FORCEINLINE int32 GetAmmo() const { return AmmoCur; }
	FORCEINLINE int32 GetMagCapacity() const { return AmmoMax; }
	FORCEINLINE void ChamberingRound();
	FORCEINLINE bool IsChamberEmpty() const { return !Chamber; };

public:
	UPROPERTY(EditAnywhere, Category = "Weapon Crosshairs")
	UTexture2D* CrosshairsCenter = nullptr;
	UPROPERTY(EditAnywhere, Category = "Weapon Crosshairs")
	UTexture2D* CrosshairsLeft = nullptr;
	UPROPERTY(EditAnywhere, Category = "Weapon Crosshairs")
	UTexture2D* CrosshairsRight = nullptr;
	UPROPERTY(EditAnywhere, Category = "Weapon Crosshairs")
	UTexture2D* CrosshairsTop = nullptr;
	UPROPERTY(EditAnywhere, Category = "Weapon Crosshairs")
	UTexture2D* CrosshairsBottom = nullptr;


	

public:
	virtual void FireRound(const FVector& _hitTarget) {}

	FORCEINLINE virtual float GetAdsFov() const { return 0.f; }
	FORCEINLINE virtual float GetAdsInterpSpeed() const { return 0.f; }
	FORCEINLINE virtual float GetFireRate() const { return 0.f; }
	FORCEINLINE virtual bool IsUsingAutoFire() const { return false; }
	FORCEINLINE virtual bool IsUsingScatter() const { return true; }
	FORCEINLINE virtual bool IsUsingScope() const { return false; }
	FORCEINLINE virtual bool IsUsingHitScan() const { return false; }
	FORCEINLINE virtual bool IsUsingMagazine() const { return true; }

	virtual void Fire(const FVector& _hitTarget) { }

	void EnableCustomDepth(bool _bEnable);



protected:
	UPROPERTY(EditAnywhere, Category = "BaseWeapon Addon")
	TSubclassOf<ACasing> CasingClass;
	UPROPERTY(EditAnywhere, Category = "BaseWeapon Addon")
	TSubclassOf<AProjectile> AmmoClass;

	virtual void EjectCasing();






public:	
	ABaseWeapon();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	// Replaicate 변수 초기화
};

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

UCLASS()
class BOUNTY_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
	/*
	* Actor setting
	*/
private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere;
	
protected:
	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:
	void ShowPickupWidget(bool _showWidget);
	void Dropped();

	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }


	/*
	* visual
	*/

private:
	UPROPERTY(EditAnywhere, Category = "Fire")
	class UAnimationAsset* FireAnimation;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;
public:
	virtual void Fire(const FVector& _hitTarget);




	/*
	* properties
	*/

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties", ReplicatedUsing = OnRep_WeaponState)	// 엑터는 replicate를 사용하려면 생성자에서 bReplicates = true 선언해줘야함
	EWeaponState WeaponState;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;
	UPROPERTY()
	class ABountyCharacter* BountyOwnerCharacter;
	UPROPERTY()
	class ABountyPlayerController* BountyOwnerController;	
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;
	UPROPERTY(EditAnywhere)
	int32 MagCapacity;
	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
private:
	UFUNCTION()
	void OnRep_WeaponState();
	UFUNCTION()
	void OnRep_Ammo();
	void SpendRound();
public:
	void SetWeaponState(EWeaponState _state);
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();
	bool IsMagEmpty();

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }




/*
* Textures for the weapon crosshair
*/
public:
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsCenter;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsBottom;

	/*
	*  Zoomed FOV while ADS
	*/
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float ZoomedFOV = 45.f;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float ZoomInterpSpeed = 30.f;
	
public:
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }





	


public:	
	ABaseWeapon();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	// Replaicate 변수 초기화
};

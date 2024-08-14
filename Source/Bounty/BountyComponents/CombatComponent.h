// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOUNTY_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	friend class ABountyCharacter;

private:
	class ABountyCharacter* Character;
	UPROPERTY(ReplicatedUsing = OnRep_EquipWeapon)
	class ABaseWeapon* EquippedWeapon;
	UPROPERTY(Replicated)
	bool bIsADS;

	UPROPERTY(EditAnywhere)
	float BaseMoveSpeed;
	UPROPERTY(EditAnywhere)
	float ADSMoveSpeed;

	bool bIsAttackHold;

protected:
	void SetADS(bool _bIsADS);
	UFUNCTION(Server, Reliable)
	void ServerSetADS(bool _bIsADS);
	UFUNCTION()
	void OnRep_EquipWeapon();

	void Attack(bool _presseed);

	UFUNCTION(Server, Reliable)
	void ServerAttack();

	// NetMulticast 옵션으로 서버에서 호출시 모든 클라이언트가 동일하게 작동함
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttack();


public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	// Replaicate 변수 설정
	void EquipWeapon(ABaseWeapon* _weaponToEquip);

public:	
	UCombatComponent();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};

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

	// NetMulticast �ɼ����� �������� ȣ��� ��� Ŭ���̾�Ʈ�� �����ϰ� �۵���
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttack();


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

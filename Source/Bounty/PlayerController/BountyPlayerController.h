// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BountyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BOUNTY_API ABountyPlayerController : public APlayerController
{
	GENERATED_BODY()	
private:
	UPROPERTY()
	class ABountyHUD* BountyHUD;

private:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float _deltaTime) override;
	virtual void OnPossess(APawn* _inPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	// Replaicate ���� �ʱ�ȭ
	virtual void ReceivedPlayer() override;	// ����Ʈ�� �ݿ� ����Ǹ� ȣ���


	/*
	* hud
	*/
private:
	class UCharacterOverlay* CharacterOverlay;
	bool bInitializeCharacterOverlay = false;
	float HUDHealthCur;
	float HUDHealthMax;
	float HUDScore;
	float HUDLifeLoss;
protected:
	void PollInit();
public:
	void SetHUD_Health(float _healthCur, float _healthMax);
	void SetHUD_Score(float _score);
	void SetHUD_LifeLoss(int32 _count);
	void SetHUD_ExtraAmmo(int32 _count);
	void SetHUD_CurrentAmmo(int32 _count);
	void SetHUD_MatchTimeCount(float _time);
	void SetHUD_WarmupTimeCount(float _time);







	/*
	* sync time between client to server
	*/
private:
	uint32 CountdownInt = 0.f;
	float LevelStartingTime = 0.f;
	float WarmupTime = 0.f;
	float MatchTime = 0.f;

protected:
	void SetHUDTime();

	// Requests the current server time, passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float _timeOfClientRequest); // Ŭ���̾�Ʈ�� ���� RPC�� ȣ��, �������� �ð��� ��� �� client RPC ȣ����
	// Reports the current server time to client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReceivedServerTime(float _timeOfClientRequest, float _timeOfServerReceived); // �����κ��� ȣ��޾Ƽ� �ð����̸� �����

	float ClientBetweenServerDelta = 0.f;

	UPROPERTY(EditAnywhere, Category = "SyncTime")
	float TimeSyncFrequency = 5.f;
	float TimeSyncRunningTime = 0.f;

	void CheckTimeSync(float _deltaTime);
public:
	// Synced with server world clock
	virtual float GetServerTime();





private:
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName BountyMatchState;	
	void HandleMatchHasStarted();
	UFUNCTION()
	void OnRep_MatchState();
public: void OnMatchStateSet(FName _state);

protected:
	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();
	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName _stateOfMatch, float _levelStartingTime, float _warmupTime, float _matchTime);
};

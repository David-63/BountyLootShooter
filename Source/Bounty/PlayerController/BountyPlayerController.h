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
	virtual void BeginPlay() override;
public:
	virtual void Tick(float _deltaTime) override;
	virtual void OnPossess(APawn* _inPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	// Replaicate 변수 초기화
	virtual void ReceivedPlayer() override;	// 뷰포트나 넷에 연결되면 호출됨


	/*
	* hud
	*/
private:
	UPROPERTY()
	class ABountyHUD* BountyHUD;
	class UCharacterOverlay* CharacterOverlay;
	bool bInitializeCharacterOverlay = false;
	float HUDHealthCur;
	float HUDHealthMax;
	float HUDScore;
	int32 HUDLifeLoss;
	int32 HUDGrenades;
protected:
	void PollInit();
public:
	void SetHUD_Health(float _healthCur, float _healthMax);
	void SetHUD_Score(float _score);
	void SetHUD_LifeLoss(int32 _count);
	void SetHUD_ExtraAmmo(int32 _count);
	void SetHUD_CurrentAmmo(int32 _count);
	void SetHUD_MatchTimeCount(float _time);
	void SetHUD_AnnouncementCount(float _time);
	void SetHUD_GrenadeCount(int32 _count);







	/*
	* sync time between client to server
	*/
private:
	uint32 CountdownInt = 0.f;
	float LevelStartingTime = 0.f;
	float WarmupTime = 0.f;
	float MatchTime = 0.f;
	float CooldownTime = 0.f;

protected:
	void SetHUDTime();

	// Requests the current server time, passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float _timeOfClientRequest); // 클라이언트가 서버 RPC를 호출, 서버에서 시간을 계산 후 client RPC 호출함
	// Reports the current server time to client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReceivedServerTime(float _timeOfClientRequest, float _timeOfServerReceived); // 서버로부터 호출받아서 시간차이를 계산함

	float ClientBetweenServerDelta = 0.f;

	UPROPERTY(EditAnywhere, Category = "SyncTime")
	float TimeSyncFrequency = 5.f;
	float TimeSyncRunningTime = 0.f;

	void CheckTimeSync(float _deltaTime);
public:
	// Synced with server world clock
	virtual float GetServerTime();




	/*
	* Handle gamemode
	*/
private:
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName BountyMatchState;	
	UPROPERTY()
	class ABountyGameMode* BountyGameMode;
	void HandleMatchHasStarted();
	void HandleCooldown();

	UFUNCTION()
	void OnRep_MatchState();
public: void OnMatchStateSet(FName _state);

protected:
	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();
	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName _stateOfMatch, float _levelStartingTime, float _warmupTime, float _matchTime, float _cooldownTime);
};

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
	
	/*
	* sync time between client and server
	*/
private:
	float MatchTime = 135.f;
	uint32 CountdownInt;
protected:
	float ClientSeverDelta = 0.f; // difference between client and server time
	UPROPERTY(EditAnywhere, Category = "Sync")
	float TimeSyncFrequency = 5.f;
	float TimeSyncRunningTime = 0.f;
protected:
	void SetHUDTime();
	// Requests the current server time, passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float _timeOfClientRequest);
	// Reports the current server time to the client in response to ServerRquestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float _timeOfClientRequest, float _timeServerReceivedClientRequest);
	void CheckTimeSync(float _deltaTime);
public:
	virtual float GetServerTime();	// Synced with server world clock





	/*
	* HUD controll
	*/
private:
	UPROPERTY()
	class ABountyHUD* BountyHUD;
public:
	virtual void OnPossess(APawn* _inPawn) override;

	void SetHUD_Health(float _healthCur, float _healthMax);
	void SetHUD_Score(float _score);
	void SetHUD_LifeLoss(int32 _count);
	void SetHUD_ExtraAmmo(int32 _count);
	void SetHUD_CurrentAmmo(int32 _count);
	void SetHUD_MatchCount(float _time);

	

	/*
	* basic
	*/
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float _deltaTime) override;
	virtual void ReceivedPlayer() override; // Sync with server clock as soon as possible

};

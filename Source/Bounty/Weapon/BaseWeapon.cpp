// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

#include "Bounty/Character/BountyCharacter.h"
#include "Bounty/BountyComponents/CombatComponent.h"
#include "Bounty/PlayerController/BountyPlayerController.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Casing.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);

	PickupArea = CreateDefaultSubobject<USphereComponent>(TEXT("PickupArea"));
	PickupArea->SetupAttachment(RootComponent);
	PickupArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickupArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (HasAuthority())
	{
		PickupArea->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PickupArea->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
	
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		PickupArea->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PickupArea->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		PickupArea->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnSphereBeginOverlap);
		PickupArea->OnComponentEndOverlap.AddDynamic(this, &ABaseWeapon::OnSphereEndOverlap);
	}

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
	MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseWeapon, WeaponState);
	DOREPLIFETIME(ABaseWeapon, AmmoCur);
}

void ABaseWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABountyCharacter* playerCharacter = Cast<ABountyCharacter>(OtherActor);
	if (playerCharacter)
	{
		playerCharacter->SetOverlappingWeapon(this);
	}
}
void ABaseWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABountyCharacter* playerCharacter = Cast<ABountyCharacter>(OtherActor);
	if (playerCharacter)
	{
		playerCharacter->SetOverlappingWeapon(nullptr);
	}
}
void ABaseWeapon::ShowPickupWidget(bool _showWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(_showWidget);
	}
}
void ABaseWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules detachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(detachRules);
	SetOwner(nullptr);
	BountyOwnerCharacter = nullptr;
	BountyOwnerController = nullptr;
}

void ABaseWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (EWeaponType::EWT_SubmachineGun == WeaponType)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		}
		EnableCustomDepth(false);
		break;
	case EWeaponState::EWS_Dropped:
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
		WeaponMesh->MarkRenderStateDirty();
		EnableCustomDepth(true);
		break;
	default:
		break;
	}
}
void ABaseWeapon::SetWeaponState(EWeaponState _state)
{
	WeaponState = _state;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		PickupArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (EWeaponType::EWT_SubmachineGun == WeaponType)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		}
		EnableCustomDepth(false);
		break;
	case EWeaponState::EWS_Dropped:
		if (HasAuthority())
		{
			PickupArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
		WeaponMesh->MarkRenderStateDirty();
		EnableCustomDepth(true);
		break;
	}
}
void ABaseWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();

	if (nullptr == Owner)
	{
		BountyOwnerCharacter = nullptr;
		BountyOwnerController = nullptr;
	}
	else
	{
		SetHUDCurrentAmmo();
	}
}
void ABaseWeapon::OnRep_Ammo()
{
	BountyOwnerCharacter = nullptr == BountyOwnerCharacter ? Cast<ABountyCharacter>(GetOwner()) : BountyOwnerCharacter;
	SetHUDCurrentAmmo();
}
void ABaseWeapon::SetHUDCurrentAmmo()
{
	BountyOwnerCharacter = nullptr == BountyOwnerCharacter ? Cast<ABountyCharacter>(GetOwner()) : BountyOwnerCharacter;
	if (BountyOwnerCharacter)
	{
		BountyOwnerController = nullptr == BountyOwnerController ? Cast<ABountyPlayerController>(BountyOwnerCharacter->Controller) : BountyOwnerController;
		if (BountyOwnerController)
		{
			if (Chamber)
			{
				BountyOwnerController->SetHUD_CurrentAmmo(AmmoCur + 1);
			}
			else
			{
				BountyOwnerController->SetHUD_CurrentAmmo(AmmoCur);
			}			
		}
	}
}

bool ABaseWeapon::IsAmmoEmpty()
{
	return 0 >= AmmoCur;
}

bool ABaseWeapon::IsAmmoFull()
{
	return AmmoCur == AmmoMax;
}

void ABaseWeapon::ChamberingRound()
{
	if (Chamber)
	{
		EjectCasing();
	}

	if (0 != AmmoCur)
	{
		Chamber = true;
	}
	else
	{
		Chamber = false;
	}
	AmmoCur = FMath::Clamp(AmmoCur - 1, 0, AmmoMax);
	SetHUDCurrentAmmo();
	UE_LOG(LogTemp, Warning, TEXT("Ammo Cur : %d"), GetAmmo());
	if (IsChamberEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("chamber Empty"));
	}
}

void ABaseWeapon::AddAmmo(int32 _ammoToAdd)
{
	AmmoCur = FMath::Clamp(AmmoCur + _ammoToAdd, 0, AmmoMax);

	SetHUDCurrentAmmo();
}

void ABaseWeapon::EnableCustomDepth(bool _bEnable)
{
	if (WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(_bEnable);
	}
}

void ABaseWeapon::EjectCasing()
{
	if (!CasingClass) return;
	if (!WeaponMesh) return;

	const USkeletalMeshSocket* ejectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
	if (ejectSocket)
	{
		FTransform socketTransform = ejectSocket->GetSocketTransform(WeaponMesh);

		UWorld* world = GetWorld();
		if (!world) return;

		FActorSpawnParameters spawnParam;
		spawnParam.Owner = this;
		ACasing* tanpi = world->SpawnActor<ACasing>(CasingClass, socketTransform.GetLocation(), socketTransform.GetRotation().Rotator(), spawnParam);

		if (!tanpi) return;
		tanpi->CasingImpulse();
	}
}

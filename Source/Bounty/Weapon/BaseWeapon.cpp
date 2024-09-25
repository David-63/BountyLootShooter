// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Bounty/Character/BountyCharacter.h"
#include "Bounty/PlayerController/BountyPlayerController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Casing.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}

}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnSphereBeginOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseWeapon::OnSphereEndOverlap);
	}

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
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
	DOREPLIFETIME(ABaseWeapon, Ammo);
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

void ABaseWeapon::Fire(const FVector& _hitTarget)
{
	if (!FireAnimation) return;
	
	WeaponMesh->PlayAnimation(FireAnimation, false);

	if (!CasingClass) return;
	USkeletalMeshComponent* weaponMesh = GetWeaponMesh();
	const USkeletalMeshSocket* ejectSocket = weaponMesh->GetSocketByName(FName("AmmoEject"));
	if (!ejectSocket) return;

	FTransform socketTransform = ejectSocket->GetSocketTransform(weaponMesh);

	UWorld* world = GetWorld();
	if (!world) return;
	
	ACasing* tanpi = world->SpawnActor<ACasing>(CasingClass, socketTransform.GetLocation(), socketTransform.GetRotation().Rotator());
	
	ABountyCharacter* owner = Cast<ABountyCharacter>(GetOwner());

	if (owner)
	{
		tanpi->SetOwnerVelocity(owner->GetVelocity());
	}
	tanpi->CasingImpulse();

	SpendRound();
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
		break;
	case EWeaponState::EWS_Dropped:
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		if (HasAuthority())
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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
		SetHUDAmmo();
	}	
}
void ABaseWeapon::OnRep_Ammo()
{
	BountyOwnerCharacter = nullptr == BountyOwnerCharacter ? Cast<ABountyCharacter>(GetOwner()) : BountyOwnerCharacter;
	SetHUDAmmo();
}
void ABaseWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo -1, 0, MagCapacity);
	SetHUDAmmo();
}
void ABaseWeapon::SetHUDAmmo()
{
	BountyOwnerCharacter = nullptr == BountyOwnerCharacter ? Cast<ABountyCharacter>(GetOwner()) : BountyOwnerCharacter;
	if (BountyOwnerCharacter)
	{
		BountyOwnerController = nullptr == BountyOwnerController ? Cast<ABountyPlayerController>(BountyOwnerCharacter->Controller) : BountyOwnerController;
		if (BountyOwnerController)
		{
			BountyOwnerController->SetHUD_Ammo(Ammo);
		}
	}
}

bool ABaseWeapon::IsMagEmpty()
{
	return 0 >= Ammo;
}




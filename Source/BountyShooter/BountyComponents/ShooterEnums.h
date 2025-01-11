#pragma once

UENUM(BlueprintType)
enum class ELoadoutSlot : uint8
{
	ELS_Primary UMETA(DisplayName = "Primary"),
	ELS_Secondary UMETA(DisplayName = "Secondary"),
	ELS_Sidearm UMETA(DisplayName = "Sidearm"),
	ELS_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EEquippedState : uint8
{
	EES_TwoHands UMETA(DisplayName = "TwoHands"),
	EES_OneHand UMETA(DisplayName = "OneHand"),
	EES_Unarmed UMETA(DisplayName = "Unarmed"),
	EES_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EGate : uint8
{
	EG_Walk UMETA(DisplayName = "Walking"),		// 완전 느리게 이동
	EG_Jog UMETA(DisplayName = "Jogging"),		// 평소 이동
	EG_Sprint UMETA(DisplayName = "Sprinting"),	// 대시 상태
	EG_Crouch UMETA(DisplayName = "Crouching"),
	EG_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class ELocomotionDirections : uint8
{
	ELD_Forward UMETA(DisplayName = "Forward"),
	ELD_Backward UMETA(DisplayName = "Backward"),
	ELD_Right UMETA(DisplayName = "Right"),
	ELD_Left UMETA(DisplayName = "Left"),
	ELD_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class ERootYawOffset : uint8
{
	ERYO_Accumulate UMETA(DisplayName = "Accumulate"),
	ERYO_BlendOut UMETA(DisplayName = "BlendOut"),
	ERYO_Hold UMETA(DisplayName = "Hold"),
	ERYO_MAX UMETA(DisplayName = "DefaultMAX"),
};


// ================================

UENUM(BlueprintType)
enum class EWeaponCategory : uint8
{
	EWC_Large UMETA(DisplayName = "Large"),
	EWC_Compact UMETA(DisplayName = "Compact"),
	EWC_Melee UMETA(DisplayName = "Melee"),
	EWC_Throwable UMETA(DisplayName = "Throwable"),
	EWC_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EWeaponSpecificType : uint8
{
	EWST_Rifle UMETA(DisplayName = "Rifle"),
	EWST_DesignatedMarksmanRifle UMETA(DisplayName = "Designated Marksman Rifle"),
	EWST_Shotgun UMETA(DisplayName = "Shotgun"),
	EWST_SubmachineGun UMETA(DisplayName = "Submachine Gun"),
	EWST_Pistol UMETA(DisplayName = "Pistol"),
	EWST_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Initial UMETA(DisplayName = "Initial State"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Dropped UMETA(DisplayName = "Dropped"),

	EIS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Weapon UMETA(DisplayName = "Weapon"),
	//EIT_Equipped UMETA(DisplayName = "Equipped"),
	//EIT_Dropped UMETA(DisplayName = "Dropped"),

	EIT_MAX UMETA(DisplayName = "DefaultMAX")
};


#define CUSTOM_DEPTH_PURPLE 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252
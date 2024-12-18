#pragma once

UENUM(BlueprintType)
enum class EInventorySlot : uint8
{
	EIS_Primary UMETA(DisplayName = "Primary"),
	EIS_Secondary UMETA(DisplayName = "Secondary"),
	EIS_Sidearm UMETA(DisplayName = "Sidearm"),
	EIS_MAX UMETA(DisplayName = "DefaultMAX"),
};

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
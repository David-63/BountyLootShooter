#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EWT_RoundsType UMETA(DisplayName = "Rounds Type"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX"),
};
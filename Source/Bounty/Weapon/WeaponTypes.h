#pragma once

#define TRACE_LENGTH 10000.f

#define CUSTOM_DEPTH_PURPLE 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_SubmachineGun UMETA(DisplayName = "Submachine Gun"),
	EWT_ScatterGun UMETA(DisplayName = "Scatter Gun"),
	EWT_MarksmanRifle UMETA(DisplayName = "Marksman Rifle"),
	EWT_GrenadeLauncher UMETA(DisplayName = "Grenade Launcher"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX"),
};


UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_Projectile UMETA(DisplayName = "Projectile"),
	EAT_HitScane UMETA(DisplayName = "Hit Scane"),
	EAT_Grenade UMETA(DisplayName = "Grenade"),
	EAT_Thruster UMETA(DisplayName = "Thruster"),
	EAT_MAX UMETA(DisplayName = "DefaultMAX"),
};
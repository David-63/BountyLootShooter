#pragma once

UENUM(BlueprintType)
enum class EMoveMode : uint8
{
	EMM_OnGround UMETA(DisplayName = "On Ground"),
	EMM_InAir UMETA(DisplayName = "In Air"),
	EMM_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	ERM_OrientToMovement UMETA(DisplayName = "Orient To Movement"),
	ERM_Strafe UMETA(DisplayName = "Strafe"),
	ERM_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_Moving UMETA(DisplayName = "Moving"),
	EMS_MAX UMETA(DisplayName = "DefaultMAX"),
};
// Gait 이거 보행을 뜻하는듯
UENUM(BlueprintType)
enum class EGaitState : uint8
{
	EGS_Walk UMETA(DisplayName = "Walk"),
	EGS_Run UMETA(DisplayName = "Run"),
	EGS_Sprint UMETA(DisplayName = "Sprint"),
	EGS_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EStanceState : uint8
{
	ESS_Stand UMETA(DisplayName = "Stand"),
	ESS_Crouch UMETA(DisplayName = "Crouch"),
	ESS_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	EMD_F UMETA(DisplayName = "F"),
	EMD_B UMETA(DisplayName = "B"),
	EMD_LL UMETA(DisplayName = "LL"),
	EMD_LR UMETA(DisplayName = "LR"),
	EMD_RL UMETA(DisplayName = "RL"),
	EMD_RR UMETA(DisplayName = "RR"),
	EMD_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EMovementDirectionBias : uint8
{
	EMDB_LeftFootForward UMETA(DisplayName = "LeftFootForward"),
	EMD_RightFootForward UMETA(DisplayName = "RightFootForward"),
	EMD_MAX UMETA(DisplayName = "DefaultMAX"),
};
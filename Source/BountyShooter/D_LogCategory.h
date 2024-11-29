#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

class BOUNTYSHOOTER_API D_LogCategory
{
public:
	static void LogInfo(const FString& Message);
	static void LogWarning(const FString& Message);
	static void LogError(const FString& Message);
};


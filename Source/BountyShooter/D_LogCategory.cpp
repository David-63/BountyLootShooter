#include "D_LogCategory.h"
#include "Misc/OutputDeviceDebug.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);


void D_LogCategory::LogInfo(const FString& Message)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Message);
        UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
    }
}

void D_LogCategory::LogWarning(const FString& Message)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, Message);
        UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
    }
}

void D_LogCategory::LogError(const FString& Message)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
        UE_LOG(LogTemp, Error, TEXT("%s"), *Message);
    }
}

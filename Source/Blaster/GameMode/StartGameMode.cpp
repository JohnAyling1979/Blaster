// Fill out your copyright notice in the Description page of Project Settings.


#include "StartGameMode.h"
#include "GameFramework/GameUserSettings.h"

AStartGameMode::AStartGameMode()
{
}

void AStartGameMode::BeginPlay()
{
    UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();

    if (GameUserSettings)
    {
        UE_LOG(LogTemp, Log, TEXT("GameUserSettings found."));
        // Run the hardware benchmark
        GameUserSettings->RunHardwareBenchmark(1, 1.0f, 1.0f);

        // Apply the detected optimal settings
        GameUserSettings->ApplyHardwareBenchmarkResults();

        // Optionally, save the settings
        GameUserSettings->SaveSettings();
    }
}

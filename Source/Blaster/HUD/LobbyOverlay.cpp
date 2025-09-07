// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyOverlay.h"
#include "Components/Button.h"

#include "Blaster/GameMode/LobbyGameMode.h"

bool ULobbyOverlay::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &ThisClass::StartButtonClicked);
	}

	return true;
}

void ULobbyOverlay::StartButtonClicked()
{
	ALobbyGameMode* GameMode = GetWorld()->GetAuthGameMode<ALobbyGameMode>();

	if (GameMode)
	{
		GameMode->StartGame();
	}
}

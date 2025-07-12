// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();

	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();

		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UMenu::MenuSetup - PlayerController is null!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UMenu::MenuSetup - World is null!"));
	}

	UGameInstance* GameInstance = GetGameInstance();

	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

		if (!MultiplayerSessionsSubsystem)
		{
			UE_LOG(LogTemp, Warning, TEXT("UMenu::MenuSetup - MultiplayerSessionsSubsystem is null!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UMenu::MenuSetup - GameInstance is null!"));
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}

	if (HostButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();

	Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("UMenu::OnCreateSession - bWasSuccessful: %s"), bWasSuccessful ? TEXT("true") : TEXT("false"));

	if (bWasSuccessful)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			World->ServerTravel(PathToLobby);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UMenu::HostButtonClicked - World is null!"));
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("Failed to create session!"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UMenu::OnCreateSession - GEngine is null!"));
		}

		HostButton->SetIsEnabled(true);
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("UMenu::OnFindSessions - bWasSuccessful: %s"), bWasSuccessful ? TEXT("true") : TEXT("false"));

	if (MultiplayerSessionsSubsystem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMenu::OnFindSessions - MultiplayerSessionsSubsystem is null!"));
		return;
	}

	for (auto Result : SessionResults)
	{
		FString SettingsMatchType;

		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsMatchType);

		if (SettingsMatchType == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}

	if (!bWasSuccessful || SessionResults.Num() <= 0)
	{
		HostButton->SetIsEnabled(true);
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();

	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();

			if (PlayerController)
			{
				UE_LOG(LogTemp, Warning, TEXT("UMenu::OnJoinSession - Joining session at address: %s"), *Address);
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UMenu::OnJoinSession - PlayerController is null!"));
			}

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UMenu::OnJoinSession - SessionInterface is not valid!"));
		}
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		HostButton->SetIsEnabled(true);
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UMenu::HostButtonClicked - MultiplayerSessionsSubsystem is null!"));
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UMenu::JoinButtonClicked - MultiplayerSessionsSubsystem is null!"));
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();

	UWorld* World = GetWorld();

	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UMenu::MenuTearDown - PlayerController is null!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UMenu::MenuTearDown - World is null!"));
	}
}

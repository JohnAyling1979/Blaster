// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MUTLIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "MultiplayerSessions | Menu")
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "MultiplayerSessions | Menu")
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION(BlueprintCallable, Category = "MultiplayerSessions | Menu")
	void OnStartSession(bool bWasSuccessful);
	UFUNCTION(BlueprintCallable, Category = "MultiplayerSessions | Menu")
	void OnDestroySession(bool bWasSuccessful);
private:
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;
	int32 NumPublicConnections{ 4 };
	FString MatchType{ TEXT("FreeForAll") };
	FString PathToLobby{ TEXT("") };

	UFUNCTION(BlueprintCallable, Category = "MultiplayerSessions | Menu")
	void HostButtonClicked();
	UFUNCTION(BlueprintCallable, Category = "MultiplayerSessions | Menu")
	void JoinButtonClicked();

	void MenuTearDown();
};

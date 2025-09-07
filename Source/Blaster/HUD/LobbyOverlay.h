// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyOverlay.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ULobbyOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UButton* StartButton;

protected:
	virtual bool Initialize() override;
	UFUNCTION()
	void StartButtonClicked();
};

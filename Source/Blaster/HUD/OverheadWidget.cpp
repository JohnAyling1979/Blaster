// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DisplayText is null in UOverheadWidget::SetDisplayText"));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)  
{  
    APlayerState* PlayerState = InPawn->GetPlayerState();

    if (!PlayerState)
    {
        SetDisplayText(TEXT("Waiting for PlayerState..."));

        if (UWorld* World = InPawn->GetWorld())
        {
            FTimerDelegate TimerDel;
            TimerDel.BindUFunction(this, FName("ShowPlayerNetRole"), InPawn);

            World->GetTimerManager().SetTimerForNextTick(TimerDel);
        }
        return;
    }

    ENetRole LocalRole = InPawn->GetLocalRole();
    FString PlayerName = PlayerState->GetPlayerName();

    FString Role;
    switch (LocalRole)
    {  
    case ENetRole::ROLE_Authority:  
        Role = TEXT("Authority");  
        break;  
    case ENetRole::ROLE_AutonomousProxy:  
        Role = TEXT("Autonomous Proxy");  
        break;  
    case ENetRole::ROLE_SimulatedProxy:  
        Role = TEXT("Simulated Proxy");  
        break;  
    case ENetRole::ROLE_None:  
        Role = TEXT("None");  
        break;  
    default:  
        Role = TEXT("Unknown Role");  
        break;  
    }  

    FString LocalRoleString = FString::Printf(TEXT("Local Role: %s\nPlayer Name: %s"), *Role, *PlayerName);  

    SetDisplayText(LocalRoleString);  
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}

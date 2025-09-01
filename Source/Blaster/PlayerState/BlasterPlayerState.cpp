// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Deaths);
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);

	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;

	if (!Character) {
		return;
	}

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;

	if (!Controller) {
		return;
	}

	Controller->SetHUDScore(GetScore());
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;

	if (!Character) {
		return;
	}

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;

	if (!Controller) {
		return;
	}

	Controller->SetHUDScore(GetScore());
}

void ABlasterPlayerState::AddToDeaths(int32 DeathAmount)
{
	Deaths += DeathAmount;

	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;

	if (!Character) {
		return;
	}

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;

	if (!Controller) {
		return;
	}

	Controller->SetHUDDeaths(Deaths);
}

void ABlasterPlayerState::OnRep_Deaths()
{
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;

	if (!Character) {
		return;
	}

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;

	if (!Controller) {
		return;
	}

	Controller->SetHUDDeaths(Deaths);
}

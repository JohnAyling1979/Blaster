// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "StartGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AStartGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AStartGameMode();

	virtual void BeginPlay() override;
};

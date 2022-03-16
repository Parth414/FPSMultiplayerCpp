// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MultiPlayerFPSGameMode.h"
#include "FPSLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERFPS_API AFPSLobbyGameMode : public AMultiPlayerFPSGameMode
{
	GENERATED_BODY()

public:
	void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting) override;

private:
	uint32 PlayerCount;
	FTimerHandle TimerHandlerQuit;
	FTimerHandle TimerHandlerStart;
	void OpenMainMenu();
	void StartGame();
};

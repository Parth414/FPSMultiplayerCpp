// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiPlayerFPSGameMode.h"
#include "MultiPlayerFPSHUD.h"
#include "MultiPlayerFPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMultiPlayerFPSGameMode::AMultiPlayerFPSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMultiPlayerFPSHUD::StaticClass();
}

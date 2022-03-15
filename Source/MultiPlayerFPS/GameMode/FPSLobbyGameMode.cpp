// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSLobbyGameMode.h"

#include "MultiPlayerFPS/FPSGameInstance.h"

void AFPSLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	++PlayerCount;
	if(HasAuthority())
	{
		GetWorldTimerManager().SetTimer(TimerHandler, this, &AFPSLobbyGameMode::OpenMainMenu, 180);
	}
	
	if(PlayerCount > 1)
	{
		UWorld* World = GetWorld();
		if (!ensure(World != nullptr)) return;
		
		World->ServerTravel("/Game/FirstPersonCPP/Maps/FirstPersonExampleMap?listen");
		UE_LOG(LogTemp, Warning, TEXT("Game Started"));
	}
}

void AFPSLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	--PlayerCount;
}

void AFPSLobbyGameMode::OpenMainMenu()
{
	UFPSGameInstance* CurrentGameInstance = Cast<UFPSGameInstance>(GetGameInstance());

	if (!ensure(CurrentGameInstance != nullptr)){return;}
	CurrentGameInstance->DestroySession();
}

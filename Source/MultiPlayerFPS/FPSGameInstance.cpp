// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameInstance.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UFPSGameInstance::UFPSGameInstance()
{
	bIsLoggedIn = false;
}

void UFPSGameInstance::Init()
{
	Super::Init();

	OnlineSubsystem = IOnlineSubsystem::Get();
	//Login();
	
}

void UFPSGameInstance::Login()
{
	if (OnlineSubsystem)
	{
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			FOnlineAccountCredentials Credentials;
			
			Credentials.Id = FString("127.0.0.1:1111");
			Credentials.Token = FString("parth414");
			Credentials.Type = FString("developer");
			/*
			Credentials.Id = FString();
			Credentials.Token = FString();
			Credentials.Type = FString("accountportal");
			*/
			Identity->OnLoginCompleteDelegates->AddUObject(this, &UFPSGameInstance::OnLoginComplete);
			Identity->Login(0,Credentials);
		}
	}
}

void UFPSGameInstance::OnLoginComplete(int32 LocalUsername, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{

	UE_LOG(LogTemp, Warning, TEXT("LoggedIn: %d"), bWasSuccessful);
	bIsLoggedIn = bWasSuccessful;
	if (OnlineSubsystem)
	{
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			Identity->ClearOnLoginCompleteDelegates(0,this);
		}
	}
}

void UFPSGameInstance::CreateSession()
{
	if (bIsLoggedIn)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				FOnlineSessionSettings SessionSettings;
				SessionSettings.bIsDedicated = false;
				SessionSettings.bShouldAdvertise = true;
				SessionSettings.bIsLANMatch = true;
				SessionSettings.NumPublicConnections = 5;
				SessionSettings.bAllowJoinInProgress = true;
				SessionSettings.bAllowJoinViaPresence = true;
				SessionSettings.bUsesPresence = true;

				SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UFPSGameInstance::OnlineCreateSessionComplete);
				SessionPtr->CreateSession(0, FName("Test Session"), SessionSettings);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CANNOT CREATE SESSION"));
	}
}

void UFPSGameInstance::OnlineCreateSessionComplete(FName SessionName, bool bWasSucccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Success: %d"), bWasSucccessful);

	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->ClearOnCreateSessionCompleteDelegates(this);
		}
	}
}

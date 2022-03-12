// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameInstance.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

const FName TestSessionName = FName("Test Sessiom");

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
				SessionSettings.bIsLANMatch = false;
				SessionSettings.NumPublicConnections = 5;
				SessionSettings.bAllowJoinInProgress = true;
				SessionSettings.bAllowJoinViaPresence = true;
				SessionSettings.bUsesPresence = true;
				SessionSettings.bUseLobbiesIfAvailable = true;
				SessionSettings.Set(SEARCH_KEYWORDS, FString("FPSLobby"), EOnlineDataAdvertisementType::ViaOnlineService);

				SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UFPSGameInstance::OnlineCreateSessionComplete);
				SessionPtr->CreateSession(0, TestSessionName/*FName("Test Session")*/, SessionSettings);
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

void UFPSGameInstance::DestroySession()
{
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UFPSGameInstance::OnlineDestroySessionComplete);
			SessionPtr->DestroySession(TestSessionName);
		}
	}
}

void UFPSGameInstance::OnlineDestroySessionComplete(FName SessionName, bool bWasSucccessful)
{
	if (OnlineSubsystem)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->ClearOnDestroySessionCompleteDelegates(this);
		}
	}
}

void UFPSGameInstance::GetAllFriends()
{
	if (bIsLoggedIn)
	{
		if(OnlineSubsystem)
		{
			if (IOnlineFriendsPtr FriendsPtr = OnlineSubsystem->GetFriendsInterface())
				{
					FriendsPtr->ReadFriendsList(0,FString(""),FOnReadFriendsListComplete::CreateUObject(this,&UFPSGameInstance::OnGetAllFriendsComplete));
				}
		}
	}
}

void UFPSGameInstance::OnGetAllFriendsComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	if (bWasSuccessful)
	{
		if (OnlineSubsystem)
		{
			if (IOnlineFriendsPtr FriendsPtr = OnlineSubsystem->GetFriendsInterface())
			{
				TArray<TSharedRef<FOnlineFriend>> FriendsList;
				if (FriendsPtr->GetFriendsList(0, ListName, FriendsList))
				{
					for (TSharedRef<FOnlineFriend> Friend : FriendsList)
					{
						FString FriendName = Friend.Get().GetRealName();
						UE_LOG(LogTemp, Warning, TEXT("Friends : %s"), *FriendName);
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed GetFriendsList"))
				}
			}
		}
	}
}

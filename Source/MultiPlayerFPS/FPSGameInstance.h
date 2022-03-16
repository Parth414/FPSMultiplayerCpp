// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FPSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERFPS_API UFPSGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UFPSGameInstance();


	virtual void Init() override;

	void Login();
	void OnLoginComplete(int32 LocalUsername, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	UFUNCTION(BlueprintCallable)
	void CreateSession();
	void OnlineCreateSessionComplete(FName SessionName, bool bWasSucccessful);

	UFUNCTION(BlueprintCallable)
	void FindSession();
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	TSharedPtr<FOnlineSessionSearch> SearchSettings;

	UFUNCTION(BlueprintCallable)
	void DestroySession();
	void OnlineDestroySessionComplete(FName SessionName, bool bWasSucccessful);

	UFUNCTION(BlueprintCallable)
	void GetAllFriends();
	void OnGetAllFriendsComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);

	UFUNCTION(BlueprintCallable)
    void LeaveGameClient();
	UFUNCTION(BlueprintCallable)
	void LeaveGameHost();
	UPROPERTY(BlueprintReadWrite)
	bool IsMainMenuOpened;
	
protected:
	class IOnlineSubsystem* OnlineSubsystem;

	bool bIsLoggedIn;
};

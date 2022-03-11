// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
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

	UFUNCTION(BlueprintCallable)
	void CreateSession();

	void OnlineCreateSessionComplete(FName SessionName, bool bWasSucccessful);

protected:
	class IOnlineSubsystem* OnlineSubsystem;
};

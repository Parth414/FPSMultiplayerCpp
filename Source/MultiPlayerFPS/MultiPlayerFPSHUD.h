// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MultiPlayerFPSHUD.generated.h"

UCLASS()
class AMultiPlayerFPSHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMultiPlayerFPSHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};


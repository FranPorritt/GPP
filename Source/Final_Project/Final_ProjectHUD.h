// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Final_ProjectHUD.generated.h"

UCLASS()
class AFinal_ProjectHUD : public AHUD
{
	GENERATED_BODY()

public:
	AFinal_ProjectHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};


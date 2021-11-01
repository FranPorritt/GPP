// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Final_ProjectGameMode.generated.h"

UCLASS(minimalapi)
class AFinal_ProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	AActor* enemy = nullptr;

	FTimerHandle endTimer;
	float endTime = 6.0f;
	
	virtual void BeginPlay() override;
	void GameOver();
	void LeaveLevel();

public:
	AFinal_ProjectGameMode();

	void PlayerCaught();
	UFUNCTION(BlueprintCallable)
	void EndGame();

	UPROPERTY(BlueprintReadWrite)
		bool playerCaught = false;
	UPROPERTY(BlueprintReadWrite)
		bool playerCompletedLevel = false;
};




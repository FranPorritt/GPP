// Copyright Epic Games, Inc. All Rights Reserved.

#include "Final_ProjectGameMode.h"
#include "Final_ProjectHUD.h"
#include "Final_ProjectCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "UObject/ConstructorHelpers.h"
#include "Director.h"
#include "Math/Color.h"


AFinal_ProjectGameMode::AFinal_ProjectGameMode()
	: Super()
{
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) == "GameLevel")
	{
		// set default pawn class to our Blueprinted character
		static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/Character/FirstPersonCharacter"));
		DefaultPawnClass = PlayerPawnClassFinder.Class;

		// use our custom HUD class
		HUDClass = AFinal_ProjectHUD::StaticClass();

		enemy = UGameplayStatics::GetActorOfClass(GetWorld(), AEnemy::StaticClass());
	}
}

void AFinal_ProjectGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::SetGamePaused(GetWorld(), false);

	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) == "GameLevel")
	{
		FTransform SpawnLocation;
		GetWorld()->SpawnActor<ADirector>(ADirector::StaticClass(), SpawnLocation);
	}
}

void AFinal_ProjectGameMode::PlayerCaught()
{
	FTimerHandle DelayTimer;
	GetWorld()->GetTimerManager().SetTimer(DelayTimer, this, &AFinal_ProjectGameMode::GameOver, 1.0f, false);	// Adds a delay so if searching locker sound gets played so the player knows the enemy searched their locker
}

void AFinal_ProjectGameMode::GameOver()
{
	UE_LOG(LogTemp, Warning, TEXT("Player Caught!"));
	UE_LOG(LogTemp, Warning, TEXT("Storing data"));

	playerCaught = true;
	Cast<ADirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ADirector::StaticClass()))->StoreData(); // Stores director data so learned behaviours are continued

	// UI Menu - Try again, quit
	Cast<AFinal_ProjectCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AFinal_ProjectCharacter::StaticClass()))->GameOverUI();
}

void AFinal_ProjectGameMode::EndGame()
{
	if (!playerCaught)
	{
		playerCompletedLevel = true;

		GetWorld()->GetTimerManager().SetTimer(endTimer, this, &AFinal_ProjectGameMode::LeaveLevel, endTime, false);
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(0.0f, 1.0f, endTime - 1.0f, FLinearColor::Black, true, true);
	}
}

void AFinal_ProjectGameMode::LeaveLevel()
{
	UE_LOG(LogTemp, Error, TEXT("load new level"));
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
}

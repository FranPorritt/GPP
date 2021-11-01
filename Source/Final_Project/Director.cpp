// Fill out your copyright notice in the Description page of Project Settings.


#include "Director.h"
#include "PointOfInterest.h"
#include "Kismet/GameplayStatics.h"
#include "Final_ProjectCharacter.h"
#include "Enemy.h"
#include "Enemy_AIController.h"
#include "Final_ProjectGameInstance.h"
#include "ItemSpawner.h"

// Sets default values
ADirector::ADirector()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADirector::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Error, TEXT("Director created"));
	InitialiseData();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APointOfInterest::StaticClass(), POIs);
	player = Cast<AFinal_ProjectCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AFinal_ProjectCharacter::StaticClass()));
	enemy = Cast<AEnemy>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemy::StaticClass()));
	enemyController = Cast<AEnemy_AIController>(enemy->GetController());
	spawner = Cast<AItemSpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), AItemSpawner::StaticClass()));

	GetWorld()->GetTimerManager().SetTimer(MenaceTimer, this, &ADirector::UpdateMenace, updateMenaceTime, true);

	AdaptDifficulty();
}

void ADirector::InitialiseData() // Gets data from game instance
{
	gameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UE_LOG(LogTemp, Error, TEXT("Initialise Data"));

	if (gameInstance) // Data persistance
	{
		playerLockerTime = Cast<UFinal_ProjectGameInstance>(gameInstance)->directorData.lockerTime;
		playerDeaths = Cast<UFinal_ProjectGameInstance>(gameInstance)->directorData.playerDeaths;
		throwableUsage = Cast<UFinal_ProjectGameInstance>(gameInstance)->directorData.throwableUses;

		if (Cast<UFinal_ProjectGameInstance>(gameInstance)->checkpointReached)
		{
			state = DirectorState::FinalHunt;
		}
	}
}

void ADirector::AdaptDifficulty()
{
	float sightRadius = 0.0f;
	float hearingRange = 0.0f;

	switch (playerDeaths) // Sets spawn rate depending on death rate
	{
	case 0:
		spawner->SetSpawnNums(8, 5);
		MAX_MENACE_FACTOR = 1.2f; // Reduce menace the more the player dies so the enemy leaves them alone more
		sightRadius = 3250.0f;
		hearingRange = 5000.0f;
		break;

	case 1:
		spawner->SetSpawnNums(12, 10);
		spawner->SpawnTraps();
		MAX_MENACE_FACTOR = 1.0f;
		sightRadius = 2850.0f;
		hearingRange = 4500.0f;
		break;

	case 2:
		spawner->SetSpawnNums(15, 15);
		spawner->SpawnTraps();
		MAX_MENACE_FACTOR = 0.85f;
		sightRadius = 2550.0f;
		hearingRange = 4250.0f;
		break;

	case 3:
		spawner->SetSpawnNums(18, 18);
		spawner->SpawnTraps();
		MAX_MENACE_FACTOR = 0.75f;
		sightRadius = 2250.0f;
		hearingRange = 4000.0f;
		break;

	case 4:
		spawner->SetSpawnNums(21, 22);
		spawner->SpawnTraps();
		MAX_MENACE_FACTOR = 0.65f;
		sightRadius = 2000.0f;
		hearingRange = 3750.0f;
		break;

	case 5:
		spawner->SetSpawnNums(24, 24);
		spawner->SpawnTraps();
		MAX_MENACE_FACTOR = 0.6f;
		sightRadius = 1850.0f;
		hearingRange = 3500.0f;
		break;

	default:
		spawner->SetSpawnNums(22, 24);
		spawner->SpawnTraps();
		MAX_MENACE_FACTOR = 0.6f;
		sightRadius = 1850.0f;
		hearingRange = 3500.0f;
		break;
	}

	// Alters enemy's sight
	enemyController->SetSightRadius(&sightRadius);
	enemyController->SetHearingRange(&hearingRange);

	if (playerLockerTime >= UNLOCK_LOCKER_BEHAVIOUR)
	{
		enemyController->UnlockBehaviour(0, true); // Unlocks ability to search lockers
	}
	if (throwableUsage >= UNLOCK_THROWABLE_KNOWLEDGE_BEHAVIOUR)
	{
		enemyController->UnlockBehaviour(1, true);
	}
}

void ADirector::StoreData()
{
	playerDeaths++; // If func is called then player caught

	Cast<UFinal_ProjectGameInstance>(gameInstance)->directorData.lockerTime = playerLockerTime;
	Cast<UFinal_ProjectGameInstance>(gameInstance)->directorData.playerDeaths = playerDeaths;
	Cast<UFinal_ProjectGameInstance>(gameInstance)->directorData.throwableUses = throwableUsage;
}

void ADirector::UpdateMenace()
{
	// Distance between enemy and player
	if (player->GetDistanceTo(enemy) <= menaceDistance) // Enemy close to player
	{
		AddMenace(addMenaceAmount);
	}

	// Can player hear enemy
	if (player->GetDistanceTo(enemy) <= hearingDistance)
	{
		AddMenace(addMenaceAmount / 2);
	}
	else
	{
		AddMenace(-0.15f);
	}
}

void ADirector::AddMenace(float value)
{
	if (state != DirectorState::FinalHunt)
	{
		if ((menaceFactor + value <= MAX_MENACE_FACTOR) && (menaceFactor + value >= 0))
		{
			menaceFactor += value;
			state = DirectorState::LowMenace;
		}
		else if (menaceFactor + value > MAX_MENACE_FACTOR)
		{
			menaceFactor += value;
			state = DirectorState::HighMenace;
		}
		else if (menaceFactor + value < 0)
		{
			menaceFactor = 0.0f;
			state = DirectorState::LowMenace;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Menace: %f"), menaceFactor);
}

//void ADirector::UpdatePOIPriorities()
//{
//	UE_LOG(LogTemp, Error, TEXT("UPDATE POI"));
//
//	if (menaceFactor < 0.2f)
//	{
//		for (AActor* p : POIs)
//		{
//			float distance = p->GetDistanceTo(player);
//
//			if (distance <= player->GetPOIRadius()) // Within radius set higher
//			{
//				Cast<APointOfInterest>(p)->UpdatePriority(2);
//			}
//			else
//			{
//				Cast<APointOfInterest>(p)->UpdatePriority(-1);
//			}
//		}
//	}
//	else if (menaceFactor < MAX_MENACE_FACTOR)
//	{
//		for (AActor* p : POIs)
//		{
//			float distance = p->GetDistanceTo(player);
//
//			if (distance <= player->GetPOIRadius()) // Within radius set higher
//			{
//				Cast<APointOfInterest>(p)->UpdatePriority(1);
//			}
//			else
//			{
//				Cast<APointOfInterest>(p)->UpdatePriority(-1);
//			}
//		}
//	}
//	else // Enemy should give player space
//	{
//		for (AActor* p : POIs)
//		{
//			float distance = p->GetDistanceTo(player);
//
//			if (distance > player->GetPOIRadius()) // Outside radius set higher
//			{
//				Cast<APointOfInterest>(p)->UpdatePriority(2);
//			}
//			else
//			{
//				Cast<APointOfInterest>(p)->UpdatePriority(-1);
//			}
//		}
//	}
//}

void ADirector::UpdateLockerTime()
{
	playerLockerTime++;
	//UE_LOG(LogTemp, Error, TEXT("Locker Time: %f"), playerLockerTime);

	if (!enemyController->GetBehaviourStatus(0)) // Get status of locker behaviour
	{
		if (playerLockerTime >= UNLOCK_LOCKER_BEHAVIOUR)
		{
			enemyController->UnlockBehaviour(0, true); // Unlocks ability to search lockers
		}
	}
}

TArray<AActor*> ADirector::GetTargetPOIs()
{
	TArray<AActor*> targetPOIs;

	switch (state)
	{
	case DirectorState::LowMenace:
		for (AActor* p : POIs)
		{
			float distance = p->GetDistanceTo(player);

			if (distance <= player->GetPOIRadius())
			{
				targetPOIs.Add(p);
			}
		}
		break;

	case DirectorState::HighMenace:
		for (AActor* p : POIs)
		{
			float distance = p->GetDistanceTo(player);

			if (distance > player->GetPOIRadius())
			{
				targetPOIs.Add(p);
			}
		}
		break;

	case DirectorState::FinalHunt:
		for (AActor* p : POIs)
		{
			float distance = p->GetDistanceTo(lift);

			if (distance <= 4000.0f)
			{
				targetPOIs.Add(p);
			}
		}
		break;

	case DirectorState::Default: // Passes far away POIs to enemy at start so it doesn't immediately head to the players area
		for (AActor* p : POIs)
		{
			float distance = p->GetDistanceTo(player);

			if (distance > player->GetPOIRadius())
			{
				targetPOIs.Add(p);
			}
		}
		break;
	}

	return targetPOIs;
}

void ADirector::TrackPlayerHiding(AFinal_ProjectCharacter& playerRef)
{
	if (player->GetHiddenStatus())
	{
		GetWorld()->GetTimerManager().SetTimer(lockerTimer, this, &ADirector::UpdateLockerTime, updateLockerTimeFreq, true);
		UE_LOG(LogTemp, Error, TEXT("TRACKING PLAYER start"));
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(lockerTimer);
		UE_LOG(LogTemp, Error, TEXT("TRACKING PLAYER stop"));
	}
}

bool ADirector::GetLockerChance()
{
	if (lockerChance < 100)
	{
		lockerChance = (playerLockerTime - UNLOCK_LOCKER_BEHAVIOUR) * 2; // eg. (19 - 15) * 2 = 8 ----- (40 - 15) * 2 = 50%
		if (lockerChance > 100)
		{
			lockerChance = 100;
		}

		int randChance = FMath::RandRange(0, 100);
		UE_LOG(LogTemp, Error, TEXT("Locker Chance: %f"), lockerChance);

		if (randChance <= lockerChance)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}

void ADirector::AddThrowableUse()
{
	throwableUsage++;

	if (throwableUsage >= UNLOCK_THROWABLE_KNOWLEDGE_BEHAVIOUR)
	{
		enemyController->UnlockBehaviour(1, true);
	}
}

void ADirector::FinalHunt(AActor* liftArg)
{
	state = DirectorState::FinalHunt;
	lift = liftArg;
	Cast<AEnemy>(enemyController->GetPawn())->UpdateState(EnemyState::Investigating);
	enemyController->PlayEnemySound(FName(TEXT("Screech")));
	UE_LOG(LogTemp, Error, TEXT("FINAL HUNT"));
}

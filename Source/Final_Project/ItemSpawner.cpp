// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSpawner.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
AItemSpawner::AItemSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItemSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	FindSpawnPoints();
	ChooseObjects();
	SpawnObjects();

	FindPickUpSpawnPoints();
	SpawnPickUps();
}

void AItemSpawner::FindSpawnPoints()
{
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "ItemSpawnPoint", SpawnPoints);

	if (SpawnPoints.Num() > 0) // Array has targets
	{
		if (numToSpawn < SpawnPoints.Num()) // Array has enough targets to spawn at
		{
			do // Chooses targets by removing unwanted locations
			{
				int randIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
				SpawnPoints.RemoveAt(randIndex);

			} 		while (SpawnPoints.Num() > numToSpawn);
			//UE_LOG(LogTemp, Warning, TEXT("Array size: %i"), SpawnPoints.Num());
		}
	}	
}

void AItemSpawner::ChooseObjects()
{
	// Loops through the array and randomly assigns them to new array, loops multiple times until new array full so multiple of same obj can be added to map
	do
	{
		for (auto obj : AllObjects)
		{
			int randomInt = FMath::RandRange(0, 1);

			if (randomInt % 2 == 0)
			{
				ObjectsToSpawn.Add(obj);
			}
		}

	} while (ObjectsToSpawn.Num() < numToSpawn);
}

void AItemSpawner::FindPickUpSpawnPoints()
{
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "PickUpSpawnPoint", PickUpSpawnPoints);

	if (PickUpSpawnPoints.Num() > 0) // Has targets
	{
		if (pickUpNumToSpawn < PickUpSpawnPoints.Num()) // Array has enough targets to spawn at
		{
			do // Chooses targets by removing unwanted locations
			{
				int randIndex = FMath::RandRange(0, PickUpSpawnPoints.Num() - 1);
				PickUpSpawnPoints.RemoveAt(randIndex);

			} while (PickUpSpawnPoints.Num() > pickUpNumToSpawn);
		}
	}
}

void AItemSpawner::SpawnTraps()
{
	TArray<AActor*> TrapSpawns;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "TrapSpawnPoint", TrapSpawns);

	int randomSpawn = FMath::RandRange(0, TrapSpawns.Num() - 1);
	SpawnTrapObj(TrapSpawns[randomSpawn]->GetActorTransform());
}

//void AItemSpawner::SpawnObjects()
//{
//	trans = SpawnPoints[0]->GetActorTransform();
//
//	for (AActor* spawnPoint : SpawnPoints)
//	{
//		int objectIndex = 0;
//		//UClass* className = ObjectsToSpawn[objectIndex].GetClass();
//		//auto obj = ObjectsToSpawn[objectIndex];
//		//UE_LOG(LogTemp, Warning, TEXT("Class: %s"), className.ToString());
//
//		// //Spawns object on spawn point
		/*FVector const location = spawnPoint->GetActorLocation();
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;
		GetWorld()->SpawnActor<testObj>(location, Rotation, SpawnInfo);*/		
//
//		objectIndex++;
//	}
//}

// Called every frame
void AItemSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



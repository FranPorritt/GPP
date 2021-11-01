// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnableObjects.h"
#include "ItemSpawner.generated.h"

UCLASS()
class FINAL_PROJECT_API AItemSpawner : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> SpawnPoints;
	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> PickUpSpawnPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TSubclassOf<class ASpawnableObjects>> AllObjects;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TSubclassOf<class ASpawnableObjects>> ObjectsToSpawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AActor> pickUpObj;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class ASpawnableObjects> trapObj;

	UPROPERTY(EditAnywhere)
		int numToSpawn = 8;
	UPROPERTY(EditAnywhere)
		int pickUpNumToSpawn = 5;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Finds all spawn points in world and then randomly removes points until left with same amount as numToSpawn
		void FindSpawnPoints();
		void ChooseObjects();
	UFUNCTION(BlueprintImplementableEvent)
		void SpawnObjects();

	void FindPickUpSpawnPoints();
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnPickUps();
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnTrapObj(FTransform trans);

public:	
	// Sets default values for this actor's properties
	AItemSpawner();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnTraps();

	void SetSpawnNums(int objNum, int pickUpNum) { numToSpawn = objNum; pickUpNumToSpawn = pickUpNum; };
};

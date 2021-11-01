// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Director.generated.h"

USTRUCT()
struct FDirectorData
{
	GENERATED_BODY()
public:
	float lockerTime;
	int playerDeaths;
	int throwableUses;
};

UENUM()
enum class DirectorState : uint8
{
	LowMenace		UMETA(DisplayName = "LowMenace"),
	HighMenace		UMETA(DisplayName = "HighMenace"),
	FinalHunt		UMETA(DisplayName = "FinalHunt"),
	Default			UMETA(DisplayName = "Default"),
};

UCLASS()
class FINAL_PROJECT_API ADirector : public AActor
{
	GENERATED_BODY()

protected:
	class AFinal_ProjectCharacter* player;
	class AEnemy* enemy;
	class AEnemy_AIController* enemyController;
	class UGameInstance* gameInstance;
	class AItemSpawner* spawner;
	class AActor* lift;

	UPROPERTY(EditAnywhere)
		DirectorState state = DirectorState::Default;
	UPROPERTY(EditAnywhere)
		TArray<AActor*> POIs;

	FTimerHandle POITimer;
	float updatePOITime = 10.0f;

	// MENACE
	FTimerHandle MenaceTimer;
	float updateMenaceTime = 2.0f; // How often menace is increased
	float menaceFactor = 0.0f;
	const float addMenaceAmount = 0.035f;
	float MAX_MENACE_FACTOR = 1.0f;
	float menaceDistance = 2500.0f;
	float hearingDistance = 3500.0f; // How far the player can hear the enemy from

	// LOCKER
	FTimerHandle lockerTimer;
	float updateLockerTimeFreq = 1.0f; // How often it updates
	float const UNLOCK_LOCKER_BEHAVIOUR = 17.5f; // How long spent in lockers before enemy unlocks ability to search them
	float playerLockerTime = 0.0f; // Tracks time spent in lockers
	float lockerChance = 0.0f;

	// THROWABLES
	int throwableUsage = 0;
	int const UNLOCK_THROWABLE_KNOWLEDGE_BEHAVIOUR = 3;

	// TRAPS
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ASpawnableObjects> trapObj;

	int playerDeaths = 0;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void InitialiseData();
	void AdaptDifficulty();

	void UpdateMenace();
	void AddMenace(float value);
	//void UpdatePOIPriorities();

	void UpdateLockerTime();

public:	
	ADirector();

	UFUNCTION(BlueprintCallable)
	void StoreData();

	TArray<AActor*> GetTargetPOIs(); // Returns a list to the enemy of target POIs dependant on current menace factor (eg. low menace - will return POIs near player)
	void TrackPlayerHiding(AFinal_ProjectCharacter& playerRef);
	bool GetLockerChance();

	void AddThrowableUse();

	UFUNCTION(BlueprintCallable)
	void FinalHunt(AActor* lift);
};
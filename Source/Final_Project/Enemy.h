// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/TargetPoint.h"
#include "Enemy.generated.h"

UENUM()
enum class EnemyState : uint8 
{
	Stalking		UMETA(DisplayName = "Stalking"),
	Searching		UMETA(DisplayName = "Seaching"),
	Chasing			UMETA(DisplayName = "Chasing"),
	Investigating	UMETA(DisplayName = "Investigating"),
};

UCLASS()
class FINAL_PROJECT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

protected:
	TArray<AActor*> spawnPoints;

	UPROPERTY(EditAnywhere)
		AActor* player;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EnemyState state = EnemyState::Searching;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Trigger Capsule")
		class UCapsuleComponent* TriggerCapsule;

	UPROPERTY(EditAnywhere)
		float killDistance = 350.0f;

	UPROPERTY(EditAnywhere)
		bool isTestLevel = false;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void CheckPlayerDistance();

public:		
	// Sets default values for this character's properties
	AEnemy();

	void Spawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void UpdateState(EnemyState newState);
	EnemyState GetEnemyState() { return state; };
	void SetSpeed(float newSpeed);
	void ChooseSpeed();

	UFUNCTION(BlueprintImplementableEvent)
		void SetStalkingAnimation();
};

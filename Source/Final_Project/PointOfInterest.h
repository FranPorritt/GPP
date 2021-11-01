// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HidingPlace.h"
#include "TimerManager.h"
#include "PointOfInterest.generated.h"

UCLASS()
class FINAL_PROJECT_API APointOfInterest : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
		TArray<AActor*> lockers;

	UPROPERTY(VisibleAnywhere, Category = "Trigger Sphere")
		class USphereComponent* TriggerSphere;
	UPROPERTY(VisibleAnywhere, Category = "Entry Sphere 1")
		class USphereComponent* EntrySphere1;	
	UPROPERTY(VisibleAnywhere, Category = "Entry Sphere 2")
		class USphereComponent* EntrySphere2;

	FTimerHandle lifeTimer;
	float lifeSpan = 10.0f; // How long POI is active before being destroyed
	FTimerDelegate playerTimerDel;
	FTimerHandle playerTimer;
	float playerUpdateTime = 10.0f; // Updates priority every 10 seconds if player remains in area

	UPROPERTY(EditAnywhere)
	float radius = 100.0f;
	UPROPERTY(EditAnywhere)
	float searchTime = 10.0f;
	UPROPERTY(EditAnywhere)
	int priority = 4;
	const int MAX_PRIORITY = 8;

	UPROPERTY(EditAnywhere)
	bool RandomPriority = false;
	bool isPlayerInArea = false;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
		virtual void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void SetBBKey(AActor* enemy, bool status);
	UFUNCTION()
		void PlayerInAreaControl();
	UFUNCTION()
		void KillPOI();

private:
	/*UPROPERTY(VisibleAnywhere)
	class UAIPerceptionStimuliSourceComponent* POIStimulus;

	void SetupStimulus();*/

public:
	// Sets default values for this actor's properties
	APointOfInterest();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float GetRadius() { return radius; };
	int GetPriority() { return priority; };
	UFUNCTION(BlueprintCallable)
	void UpdatePriority(int num);

	TArray<AActor*> GetLockerArray() { return lockers; };

	void StartLifeTimer();
};

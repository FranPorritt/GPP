// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "HidingPlace.generated.h"
UCLASS()
class FINAL_PROJECT_API AHidingPlace : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void HideControl();
	UFUNCTION()
		void Hide();
	UFUNCTION()
		void Leave();
	UFUNCTION(BlueprintImplementableEvent)
		void RotatePlayer(FRotator newRot);
	UFUNCTION(BlueprintImplementableEvent)
		void LockerSound();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Trigger Box")
		class UBoxComponent* TriggerBox;

	/*UPROPERTY (VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
		class UCameraComponent* HidingCamera;*/

	UPROPERTY(VisibleAnywhere, Category = "Player Pos Sphere")
		class USphereComponent* PlayerSphere;
	UPROPERTY(VisibleAnywhere, Category = "Exit Sphere")
		class USphereComponent* ExitSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AFinal_ProjectCharacter* player;

	bool allowInput = false;
	UPROPERTY(BlueprintReadWrite)
	bool playerHiding = false;
	
public:	
	// Sets default values for this actor's properties
	AHidingPlace();

	bool GetHidingStatus() { return playerHiding; };
};

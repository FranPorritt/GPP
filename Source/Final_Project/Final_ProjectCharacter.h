// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Sound/SoundBase.h"
#include "Final_ProjectCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AFinal_ProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* L_MotionController;

public:
	AFinal_ProjectCharacter();

protected:
	virtual void BeginPlay();

	/** Fires a projectile. */
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	UFUNCTION(BlueprintImplementableEvent)
		void PlayFootsteps();

	void CrouchControl();
	void Sprint();
	void StopSprinting();

	UFUNCTION(BlueprintImplementableEvent)
	void ClampControl();
	UFUNCTION(BlueprintCallable)
	void FootstepEvent(); // Spawns noise event for AI sense
	UFUNCTION(BlueprintImplementableEvent)
	void PlayerReceivedKeycard();

	UPROPERTY(BlueprintReadWrite)
	class ADirector* director;

	FVector defaultCameraPos; // For crouching
	UPROPERTY(EditAnywhere)
	AActor* checkpointSpawn;

	AActor* currentLocker = nullptr; // Locker player is hiding in

	float POIRadius = 3500.0f;

	float defaultSpeed = 0.0f;
	float sprintSpeed = 0.0f;

	int ammo = 2;

	UPROPERTY(BlueprintReadWrite)
	bool hasKeyCard = false;
	UPROPERTY(BlueprintReadWrite)
	bool isHiding = false;	
	UPROPERTY(BlueprintReadWrite)
	bool flashlightStatus = false;
	UPROPERTY(BlueprintReadWrite)
	bool isSprinting = false;
	UPROPERTY(BlueprintReadWrite)
	bool fireEnabled = true;

private:
	UPROPERTY(EditAnywhere)
	class UAIPerceptionStimuliSourceComponent* stimulus;

	void SetupStimulus();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Projectile class to spawn */
	UPROPERTY(EditAnywhere, Category = Projectile)
		TSubclassOf<class AFinal_ProjectProjectile> ProjectileClass;

	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	void Spawn();
	UFUNCTION(BlueprintImplementableEvent)
		void GameOverUI();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UseUI();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void HideUseUI();

	void GetKeyCard() { hasKeyCard = true; PlayerReceivedKeycard(); }; // Gives player key card
	UFUNCTION(BlueprintCallable)
	bool GetKeyStatus() { return hasKeyCard; }; // Returns whether the player has the key card

	void HideControl();
	bool GetHiddenStatus() { return isHiding; };
	void SetHidingRotation(FQuat newRot) { SetActorRotation(newRot); };
	void SetCurrentLocker(AActor* lockerArg) { currentLocker = lockerArg; };
	AActor* GetCurrentLocker() { return currentLocker; };
	bool GetFlashlightStatus() { return flashlightStatus; };

	UFUNCTION(BlueprintImplementableEvent)
	void LockerQTE(); // Locker quick time event if enemy searching
	UPROPERTY(BlueprintReadWrite)
	bool QTESuccess = false;

	float GetPOIRadius() { return POIRadius; };

	UFUNCTION(BlueprintCallable)
	void AddAmmo() { ammo++; };
	UFUNCTION(BlueprintCallable)
	int GetAmmo() { return ammo; };
};


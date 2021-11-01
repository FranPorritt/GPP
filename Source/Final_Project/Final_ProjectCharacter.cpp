// Copyright Epic Games, Inc. All Rights Reserved.

#include "Final_ProjectCharacter.h"
#include "Final_ProjectProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Director.h"
#include "Final_ProjectGameInstance.h"


DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFinal_ProjectCharacter

AFinal_ProjectCharacter::AFinal_ProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	SetupStimulus();
}

void AFinal_ProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (Cast<UFinal_ProjectGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->checkpointReached) // If player reached checkpoint last attempt
	{
		hasKeyCard = true;
		PlayerReceivedKeycard(); // Sets all keycard UI bits
		Spawn();
	}
	
	defaultCameraPos = FirstPersonCameraComponent->GetRelativeLocation();
	defaultSpeed = GetCharacterMovement()->MaxWalkSpeed;
	sprintSpeed = defaultSpeed * 1.75;

	director = Cast<ADirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ADirector::StaticClass()));
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFinal_ProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFinal_ProjectCharacter::CrouchControl);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AFinal_ProjectCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AFinal_ProjectCharacter::StopSprinting);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFinal_ProjectCharacter::OnFire);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFinal_ProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFinal_ProjectCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFinal_ProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFinal_ProjectCharacter::LookUpAtRate);
}

void AFinal_ProjectCharacter::SetupStimulus()
{
	stimulus = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	stimulus->RegisterForSense(TSubclassOf<UAISense_Sight>());
	stimulus->RegisterForSense(TSubclassOf<UAISense_Hearing>());
	stimulus->RegisterWithPerceptionSystem();
}

void AFinal_ProjectCharacter::Spawn()
{
	if (checkpointSpawn)
	{
		SetActorLocation(checkpointSpawn->GetActorLocation());
	}
}

void AFinal_ProjectCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);

		PlayFootsteps();
	}
}

void AFinal_ProjectCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);

		PlayFootsteps();
	}
}

void AFinal_ProjectCharacter::FootstepEvent()
{
	if (GetCharacterMovement()->GetMovementName() != "MOVE_None")
	{
		float range = 1850.0f;

		if (isSprinting)
		{
			range = 3000.0f;
		}
		else if (bIsCrouched)
		{
			range = 1000.0f;
		}

		FVector const location = GetActorLocation();
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), location, 1.0f, this, range, "Noise");
	}
}

void AFinal_ProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFinal_ProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFinal_ProjectCharacter::OnFire()
{
	if (fireEnabled)
	{
		if (ammo > 0)
		{
			// try and fire a projectile
			if (ProjectileClass != NULL)
			{
				UWorld* const World = GetWorld();
				if (World != NULL)
				{

					//const FRotator SpawnRotation = GetControlRotation();	
					const FRotator SpawnRotation = FirstPersonCameraComponent->GetComponentRotation();
					const FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 65);

					//Set Spawn Collision Handling Override
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

					// spawn the projectile
					World->SpawnActor<AFinal_ProjectProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
				}
			}

			ammo--;
		}
	}
}

void AFinal_ProjectCharacter::CrouchControl()
{
	if (!bIsCrouched)
	{
		Crouch();
		FirstPersonCameraComponent->SetRelativeLocation(FVector(defaultCameraPos.X, defaultCameraPos.Y, defaultCameraPos.Z - 50.0f), false, 0, ETeleportType::None);
	}
	else
	{
		UnCrouch();
		FirstPersonCameraComponent->SetRelativeLocation(defaultCameraPos, false, 0, ETeleportType::None);
	}
}

void AFinal_ProjectCharacter::Sprint()
{
	isSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = sprintSpeed;
}

void AFinal_ProjectCharacter::StopSprinting()
{
	isSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = defaultSpeed;
}

void AFinal_ProjectCharacter::HideControl()
{
	if (isHiding) // Is hidden and now leaving hiding spot
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
		ClampControl();
		currentLocker = nullptr;
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_None);
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
		ClampControl();
		
		if (bIsCrouched)
		{
			CrouchControl();
		}
	}

	isHiding = !isHiding;
	director->TrackPlayerHiding(*this); // Tells director when to track players time hiding
}

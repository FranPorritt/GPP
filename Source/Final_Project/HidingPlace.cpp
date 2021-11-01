// Fill out your copyright notice in the Description page of Project Settings.


#include "HidingPlace.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Final_ProjectCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHidingPlace::AHidingPlace()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	TriggerBox->InitBoxExtent(FVector(10.0f, 6.0f, 1.0f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = TriggerBox;

	PlayerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Player Pos Sphere"));
	PlayerSphere->SetCollisionProfileName(TEXT("NoCollision"));
	PlayerSphere->SetupAttachment(RootComponent);

	ExitSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Exit Sphere"));
	ExitSphere->SetCollisionProfileName(TEXT("NoCollision"));
	ExitSphere->SetupAttachment(RootComponent);

	Tags.Add("HidingPlace");
}

// Called when the game starts or when spawned
void AHidingPlace::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<AFinal_ProjectCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AFinal_ProjectCharacter::StaticClass()));
}

void AHidingPlace::HideControl()
{
	if (!playerHiding)
	{
		Hide();
	}
	else
	{
		Leave();
	}
}

void AHidingPlace::Hide()
{
	playerHiding = true;
	LockerSound();

	player->SetActorLocation(FMath::Lerp(player->GetActorLocation(), PlayerSphere->GetComponentLocation(), 1.0f), false, 0, ETeleportType::None);
	player->SetCurrentLocker(this);
	FRotator currentRot = GetActorRotation();
	RotatePlayer(currentRot);
	Cast<AFinal_ProjectCharacter>(player)->HideControl();
}

void AHidingPlace::Leave()
{
	playerHiding = false;
	LockerSound();

	player->SetActorLocation(FMath::Lerp(player->GetActorLocation(), ExitSphere->GetComponentLocation(), 1.0f), false, 0, ETeleportType::None);
	Cast<AFinal_ProjectCharacter>(player)->HideControl();
}
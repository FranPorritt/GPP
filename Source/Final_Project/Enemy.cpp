// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Final_ProjectGameMode.h"


// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);

	Tags.Add("Enemy");
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (!isTestLevel)
	{
		Spawn();
	}
}

void AEnemy::Spawn()
{
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "EnemySpawn", spawnPoints);
	int spawnIndex = FMath::RandRange(0, spawnPoints.Num() - 1);
	SetActorLocation(spawnPoints[spawnIndex]->GetActorLocation());
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (state == EnemyState::Chasing)
	{
		CheckPlayerDistance();
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherActor->ActorHasTag("Player"))
	{
		Cast<AFinal_ProjectGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->PlayerCaught();
	}
}

void AEnemy::CheckPlayerDistance()
{
	if (FVector::Distance(GetActorLocation(), player->GetActorLocation()) <= killDistance)
	{
		Cast<AFinal_ProjectGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->PlayerCaught();
	}
}

void AEnemy::UpdateState(EnemyState newState)
{
	state = newState;

	ChooseSpeed();
}

void AEnemy::SetSpeed(float newSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = newSpeed;
}

void AEnemy::ChooseSpeed()
{
	float newSpeed = 0.0f;

	switch (state)
	{
	case EnemyState::Searching:
		newSpeed = FMath::RandRange(300, 550);
		break;

	case EnemyState::Chasing:
		newSpeed = FMath::RandRange(1400, 1600);
		break;

	case EnemyState::Investigating:
		newSpeed = FMath::RandRange(650, 850);
		break;
	
	case EnemyState::Stalking:
		newSpeed = FMath::RandRange(250, 320);
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed = newSpeed;
}

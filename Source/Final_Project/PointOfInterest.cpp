// Fill out your copyright notice in the Description page of Project Settings.


#include "PointOfInterest.h"
#include "Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "BlackboardKeys.h"
#include "Enemy_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

// Sets default values
APointOfInterest::APointOfInterest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger Sphere"));
	TriggerSphere->SetCollisionProfileName(TEXT("Trigger"));
	SetRootComponent(TriggerSphere);
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &APointOfInterest::OnOverlapBegin);

	EntrySphere1 = CreateDefaultSubobject<USphereComponent>(TEXT("Entry Sphere 1"));
	EntrySphere1->SetCollisionProfileName(TEXT("Trigger"));
	EntrySphere1->SetSphereRadius(50.0f);
	EntrySphere1->SetupAttachment(RootComponent);
	EntrySphere1->OnComponentBeginOverlap.AddDynamic(this, &APointOfInterest::OnOverlapBegin);

	EntrySphere2 = CreateDefaultSubobject<USphereComponent>(TEXT("Entry Sphere 2"));
	EntrySphere2->SetCollisionProfileName(TEXT("Trigger"));
	EntrySphere2->SetSphereRadius(50.0f);
	EntrySphere2->SetupAttachment(RootComponent);
	EntrySphere2->OnComponentBeginOverlap.AddDynamic(this, &APointOfInterest::OnOverlapBegin);

	Tags.Add("POI");
}

// Called when the game starts or when spawned
void APointOfInterest::BeginPlay()
{
	Super::BeginPlay();

	if (RandomPriority)
	{
		priority = FMath::RandRange(3, MAX_PRIORITY - 2);
	}

	//SetupStimulus();

	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &APointOfInterest::OnOverlapEnd);
}

//void APointOfInterest::SetupStimulus()
//{
//	POIStimulus = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("POI Stimulus"));
//	POIStimulus->RegisterForSense(TSubclassOf<UAISense_Sight>());
//	POIStimulus->RegisterWithPerceptionSystem();
//}

// Called every frame
void APointOfInterest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void APointOfInterest::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, (TEXT("POI TRIGGER")));
	if (OverlappedComponent == TriggerSphere)
	{
		if (OtherActor && (OtherActor != this) && OtherActor->ActorHasTag("Enemy"))
		{
			UE_LOG(LogTemp, Warning, (TEXT("ENEMY-POI")));
			SetBBKey(OtherActor, true);
		}
	}

	if ((OverlappedComponent == EntrySphere1) || (OverlappedComponent == EntrySphere2))
	{
		if (OtherActor && (OtherActor != this) && OtherActor->ActorHasTag("Player")) // Increases priority of POI player has entered
		{
			UE_LOG(LogTemp, Warning, TEXT("%s - PLAYER-POI"), *UKismetSystemLibrary::GetDisplayName(this));
			PlayerInAreaControl();
		}
	}
}

void APointOfInterest::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlappedComp == TriggerSphere)
	{
		if (OtherActor && (OtherActor != this) && OtherActor->ActorHasTag("Enemy"))
		{
			//UE_LOG(LogTemp, Warning, (TEXT("exit POI")));
			SetBBKey(OtherActor, false);
		}
	}
}

void APointOfInterest::SetBBKey(AActor* enemy, bool status)
{
	AEnemy* Enemy = Cast<AEnemy>(enemy);
	AEnemy_AIController* controller = Cast<AEnemy_AIController>(Enemy->GetController());

	controller->GetBlackboard()->SetValueAsBool(bb_keys::is_in_POI, status);
}

void APointOfInterest::PlayerInAreaControl()
{
	if (isPlayerInArea) // Player already in area when called
	{
		UE_LOG(LogTemp, Warning, (TEXT("player leave POI")));
		GetWorld()->GetTimerManager().ClearTimer(playerTimer); // Stops priority timer
	}
	else
	{
		UE_LOG(LogTemp, Warning, (TEXT("player enter POI")));
		UpdatePriority(1);
		// Calls updatePriority every 10 seconds if the player remains in the area
		playerTimerDel.BindUFunction(this, FName("UpdatePriority"), 1);
		GetWorldTimerManager().SetTimer(playerTimer, playerTimerDel, playerUpdateTime, true);
	}

	isPlayerInArea = !isPlayerInArea;
}

void APointOfInterest::UpdatePriority(int num)
{
	if ((priority + num <= MAX_PRIORITY) && (priority + num > 0))
	{
		priority += num;
	}
	else if (priority + num >= MAX_PRIORITY)
	{
		priority = MAX_PRIORITY;
	}
	else if ((priority + num <= 0))
	{
		priority = 1;
	}
}

void APointOfInterest::StartLifeTimer()
{
	GetWorld()->GetTimerManager().SetTimer(lifeTimer, this, &APointOfInterest::KillPOI, lifeSpan, false);
}

void APointOfInterest::KillPOI()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	Destroy();
}


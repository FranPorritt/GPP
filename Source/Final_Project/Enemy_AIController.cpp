// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy_AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Final_ProjectCharacter.h"
#include "BlackboardKeys.h"
#include "Perception/PawnSensingComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "GameFramework/Character.h"
#include "PointOfInterest.h"
#include "Enemy.h"
#include "Director.h"
#include "Final_ProjectGameMode.h"

AEnemy_AIController::AEnemy_AIController(FObjectInitializer const& object_initializer)
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/FirstPersonCPP/Blueprints/AI/Enemy_BT.Enemy_BT'"));
	//static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/FirstPersonCPP/Blueprints/AI/TEST_Enemy_BT.TEST_Enemy_BT'")); // TEST LEVEL
	if (obj.Succeeded())
	{
		behaviorTree = obj.Object;
	}

	behaviorTreeComponent = object_initializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorTreeComp"));
	blackboard = object_initializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));

	SetupPerceptionSystem();
}

void AEnemy_AIController::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(behaviorTree);
	behaviorTreeComponent->StartTree(*behaviorTree);
	
	GetBlackboard()->SetValueAsBool(bb_keys::is_start, true);
	GetWorld()->GetTimerManager().SetTimer(startBehaviourTimer, this, &AEnemy_AIController::EndStart, 2.0f, false);

	director = Cast<ADirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ADirector::StaticClass())); // Finds director actor and casts to director class
}

void AEnemy_AIController::OnPossess(APawn* const pawn)
{
	Super::OnPossess(pawn);
	if (blackboard)
	{
		blackboard->InitializeBlackboard(*behaviorTree->BlackboardAsset);
	}
}

UBlackboardComponent* AEnemy_AIController::GetBlackboard() const
{
	return blackboard;
}

void AEnemy_AIController::SetupPerceptionSystem()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception Component"));
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	hearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));

	sightConfig->SightRadius = sightRadius;
	sightConfig->LoseSightRadius = (sightConfig->SightRadius + 1500.0f);
	sightConfig->PeripheralVisionAngleDegrees = 160.0f;
	sightConfig->DetectionByAffiliation.bDetectEnemies = true;
	sightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	PerceptionComponent->ConfigureSense(*sightConfig);

	hearingConfig->HearingRange = hearingRange;
	hearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	hearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	hearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	PerceptionComponent->ConfigureSense(*hearingConfig);

	PerceptionComponent->SetDominantSense(sightConfig->GetSenseImplementation());
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemy_AIController::OnTargetDetected);
}

void AEnemy_AIController::OnTargetDetected(AActor* actor, FAIStimulus const stimulus)
{
	if (stimulus.Tag == "Noise")
	{
		UE_LOG(LogTemp, Error, TEXT("Hears something"));
		CheckNoise(actor);

		GetBlackboard()->SetValueAsBool(bb_keys::is_investigating, stimulus.WasSuccessfullySensed()); 
	}
	else
	{
		if (actor->ActorHasTag("Flashlight")) // Enemy sees light from flashlight
		{
			UE_LOG(LogTemp, Error, TEXT("Sees flashlight"));

			if (Cast<AFinal_ProjectCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->GetFlashlightStatus()) // If flashlight is turned on
			{
				flashlight = actor;
				GetBlackboard()->SetValueAsBool(bb_keys::is_stalking, stimulus.WasSuccessfullySensed());

				// Check if player in locker
			}
		}

		if (auto const ch = Cast<AFinal_ProjectCharacter>(actor))
		{
			if (!ch->GetHiddenStatus()) // If player isn't hiding in locker
			{
				GetBlackboard()->SetValueAsBool(bb_keys::seen_player, true); // Once seen, cannot be unseen
				PlayEnemySound(FName(TEXT("Screech")));
				CheckDistance(actor);
			}			
		}

		if (actor->ActorHasTag("POI"))
		{
			TESTCallStim(actor); // Raises priority of seen POIs
		}
	}
}

void AEnemy_AIController::CheckDistance(AActor* actor)
{
	if (FVector::Distance(actor->GetActorLocation(), GetPawn()->GetActorLocation()) < 350.0f)
	{
		Cast<AFinal_ProjectGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->PlayerCaught();
	}
}

void AEnemy_AIController::CheckNoise(AActor* actor)
{
	if (actor)
	{
		if (actor->ActorHasTag("Throwable")) 
		{
			if (behaviours.ThrowableKnowledgeUnlocked)
			{
				// Run to noise
				// Spawn noise in players direction

				SpawnNoisePOI(actor);
				GetBlackboard()->SetValueAsBool(bb_keys::knows_throwable, true);
				currentThrowableObject = actor;
			}
			else
			{
				SpawnNoisePOI(actor);
				GetBlackboard()->SetValueAsBool(bb_keys::knows_throwable, false);
			}
			director->AddThrowableUse();
		}
		else if (actor->ActorHasTag("Player"))
		{
			if (!investigatingPlayerNoise)
			{
				UE_LOG(LogTemp, Error, TEXT("Spawning player POI"));
				investigatingPlayerNoise = true;
				SpawnNoisePOI(actor);
				ResetPlayerInvestigate();
			}
		}
		else if (actor->ActorHasTag("HidingPlace"))
		{
			// If enemy hears player enter locker will run towards it and search it immediately
			UE_LOG(LogTemp, Error, TEXT("Heard locker!"));
			currentTargetLocker = actor;

			FNavLocation location;
			// Get nav system and generate a random location on NavMesh
			UNavigationSystemV1* const nav_sys = UNavigationSystemV1::GetCurrent(GetWorld());
			if (nav_sys->GetRandomPointInNavigableRadius(actor->GetActorLocation(), 250.0f, location, nullptr))
			{
				GetBlackboard()->SetValueAsVector(bb_keys::target_location, location.Location);
			}

			GetBlackboard()->SetValueAsBool(bb_keys::heard_locker, true);
		}
		else
		{
			SpawnNoisePOI(actor);
		}
	}
}

void AEnemy_AIController::SpawnNoisePOI(AActor* actor)
{
	// Spawns POI on noise location and sets as current target
	FVector const location = actor->GetActorLocation();
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;
	APointOfInterest* noisePOI = GetWorld()->SpawnActor<APointOfInterest>(location, Rotation, SpawnInfo);
	noisePOI->StartLifeTimer();
	currentTargetPOI = noisePOI;
	GetBlackboard()->SetValueAsObject(bb_keys::target_poi, currentTargetPOI);
}

void AEnemy_AIController::EndStart()
{
	GetBlackboard()->SetValueAsBool(bb_keys::is_start, false);
}

void AEnemy_AIController::UnlockBehaviour(int key, bool status)
{
	switch (key)
	{
	case 0:
		behaviours.SearchLockersUnlocked = status;
		GetBlackboard()->SetValueAsBool(bb_keys::search_lockers_unlocked, true);
		UE_LOG(LogTemp, Error, TEXT("Controller: Unlock locker search"));
		break;

	case 1:
		behaviours.ThrowableKnowledgeUnlocked = status;
		UE_LOG(LogTemp, Error, TEXT("Controller: Unlock throwable knowledge"));
		break;

	default:
		break;
	}
}

bool AEnemy_AIController::GetBehaviourStatus(int key)
{
	switch (key)
	{
	case 0:
		return behaviours.SearchLockersUnlocked;
		break;

	case 1:
		return behaviours.ThrowableKnowledgeUnlocked;
		break;

	default:
		break;
	}

	return false;
}

AActor* AEnemy_AIController::ChooseTargetPOI()
{
	TArray<AActor*> POIs = director->GetTargetPOIs();

	if (POIs.Num() > 0)
	{
		// Finds the current highest priority attached to any POI
		int highestPriority = 0;
		AActor* targetPoint = nullptr;
		for (AActor* p : POIs)
		{
			if (p != currentTargetPOI)
			{
				APointOfInterest* poi = Cast<APointOfInterest>(p);
				if (highestPriority < poi->GetPriority())
				{
					highestPriority = poi->GetPriority();
				}
			}
		}

		// Finds all POIs with the highest priority
		TArray<AActor*> PriorityPOIs;
		for (AActor* p : POIs)
		{
			if (p != currentTargetPOI)
			{
				APointOfInterest* poi = Cast<APointOfInterest>(p);
				if (poi->GetPriority() == highestPriority)
				{
					PriorityPOIs.Add(p);
				}
			}
		}

		// Gets nearest POI from the highest priorities
		float closestDistance = 10000.0f;
		if (PriorityPOIs.Num() > 1)
		{
			for (AActor* p : PriorityPOIs)
			{
				if (p != currentTargetPOI)
				{
					float distance = p->GetDistanceTo(GetPawn());

					if (closestDistance > distance)
					{
						closestDistance = distance;
						targetPoint = p;
					}
				}
			}
		}
		else if (PriorityPOIs.Num() == 1)
		{
			targetPoint = PriorityPOIs[0]; // Only 1 in array
		}

		currentTargetPOI = targetPoint;
		GetBlackboard()->SetValueAsObject(bb_keys::target_poi, currentTargetPOI);

		if (!targetPoint)
		{
			return nullptr;
		}

		return targetPoint;
	}

	return nullptr;
}

void AEnemy_AIController::SortArray(TArray<AActor*> array)
{
	FVector enemyLocation = GetPawn()->GetActorLocation();
	array.Sort([enemyLocation](const AActor& A, const AActor& B)
	{
		float distanceA = FVector::DistSquared(enemyLocation, A.GetActorLocation());
		float distanceB = FVector::DistSquared(enemyLocation, B.GetActorLocation());

		return distanceA > distanceB;
	});
}

AActor* AEnemy_AIController::FindLocker()
{
	if (currentTargetPOI)
	{
		TArray<AActor*> lockers = Cast<APointOfInterest>(currentTargetPOI)->GetLockerArray(); 

		if (lockers.Num() > 0)
		{
			int lockerIndex = FMath::RandRange(0, lockers.Num() - 1);
			currentTargetLocker = lockers[lockerIndex];
			return currentTargetLocker;
		}
	}

	return nullptr;
}

void AEnemy_AIController::LockerQTE()
{
	auto const player = Cast<AFinal_ProjectCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (player)
	{
		UE_LOG(LogTemp, Error, TEXT("QTE"));
		player->LockerQTE();

		FTimerHandle qteTimer;
		GetWorld()->GetTimerManager().SetTimer(qteTimer, this, &AEnemy_AIController::CheckQTESuccess, 3.0f, false);		
	}
}


void AEnemy_AIController::CheckQTESuccess()
{
	auto const player = Cast<AFinal_ProjectCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	UE_LOG(LogTemp, Error, TEXT("check success"));
	if (!player->QTESuccess)
	{
		PlayEnemySound(FName(TEXT("Locker")));
		Cast<AFinal_ProjectGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->PlayerCaught();
	}
	else
	{
		GetBlackboard()->SetValueAsBool(bb_keys::locker_flashlight, false);
	}
}
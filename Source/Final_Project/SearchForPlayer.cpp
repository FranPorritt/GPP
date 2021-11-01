// Fill out your copyright notice in the Description page of Project Settings.


#include "SearchForPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Enemy_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "BlackboardKeys.h"
#include "Final_ProjectProjectile.h"
#include "PointOfInterest.h"

USearchForPlayer::USearchForPlayer(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Search for player");
}

EBTNodeResult::Type USearchForPlayer::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	// Get AI Controller
	auto const controller = Cast<AEnemy_AIController>(owner_comp.GetAIOwner());
	AActor* throwableObj = controller->currentThrowableObject;

	if (throwableObj)
	{
		// Finds random location in direction obj was thrown from
		FVector objLocation = throwableObj->GetActorLocation();
		FVector startLocation = Cast<AFinal_ProjectProjectile>(throwableObj)->GetStartPos(); 

		FVector const targetLocation = (startLocation + ((objLocation - startLocation) / 2)) * FVector(1,1,0); // Finds midpoint between start and end of thrown object, times (1,1,0) so z is 0
		// Spawns POI on noise location and sets as current target
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;
		APointOfInterest* noisePOI = GetWorld()->SpawnActor<APointOfInterest>(targetLocation, Rotation, SpawnInfo);
		noisePOI->StartLifeTimer();
		controller->SetActiveTarget(noisePOI);

		controller->GetBlackboard()->SetValueAsBool(bb_keys::knows_throwable, false); // Unsets bool so it can continue on to finding a random point in new POI

		FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	FinishLatentTask(owner_comp, EBTNodeResult::Failed);
	return EBTNodeResult::Failed;
}


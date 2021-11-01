// Fill out your copyright notice in the Description page of Project Settings.


#include "FindRandomLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Enemy_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Enemy.h"

UFindRandomLocation::UFindRandomLocation(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Find random location");
}

EBTNodeResult::Type UFindRandomLocation::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	// Get AI Controller and Enemy
	auto const controller = Cast<AEnemy_AIController>(owner_comp.GetAIOwner());
	auto const enemy = controller->GetPawn();

	// Get enemy location
	FVector const origin = enemy->GetActorLocation();
	FNavLocation location;

	// Get nav system and generate a random location on NavMesh
	UNavigationSystemV1* const nav_sys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (nav_sys->GetRandomPointInNavigableRadius(origin, searchRadius, location, nullptr))
	{
		controller->GetBlackboard()->SetValueAsVector("TargetLocation", location.Location);
	}

	Cast<AEnemy>(controller->GetPawn())->UpdateState(EnemyState::Searching); // Updates enemy state

	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

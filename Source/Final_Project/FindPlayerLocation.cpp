// Fill out your copyright notice in the Description page of Project Settings.


#include "FindPlayerLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Enemy_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "BlackboardKeys.h"
#include "Enemy.h"

UFindPlayerLocation::UFindPlayerLocation(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Find player location");
}

EBTNodeResult::Type UFindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	// Get AI Controller and Player
	auto const controller = Cast<AEnemy_AIController>(owner_comp.GetAIOwner());
	ACharacter* const player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	FVector const playerLocation = player->GetActorLocation();

	FNavLocation location;
	// Get nav system and generate a random location on NavMesh
	UNavigationSystemV1* const nav_sys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (nav_sys->GetRandomPointInNavigableRadius(playerLocation, searchRadius, location, nullptr))
	{
		controller->GetBlackboard()->SetValueAsVector(bb_keys::target_location, location.Location);
	}

	if (Cast<AEnemy>(controller->GetPawn())->GetEnemyState() == EnemyState::Stalking) // If enemy was stalking when spotted player -- will continue stalking animation
	{
		Cast<AEnemy>(controller->GetPawn())->SetStalkingAnimation();
	}

	Cast<AEnemy>(controller->GetPawn())->UpdateState(EnemyState::Chasing); // Updates enemy state

	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

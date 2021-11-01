// Fill out your copyright notice in the Description page of Project Settings.


#include "InvestigateNoise.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Enemy_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "BlackboardKeys.h"
#include "PointOfInterest.h"
#include "Enemy.h"

UInvestigateNoise::UInvestigateNoise(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Investigate");
}

EBTNodeResult::Type UInvestigateNoise::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	// Get AI Controller
	auto const controller = Cast<AEnemy_AIController>(owner_comp.GetAIOwner());
	AActor* const POITarget = controller->GetActiveTarget();
	FVector const targetLocation = POITarget->GetActorLocation();

	FNavLocation location;
	// Get nav system and generate a random location on NavMesh
	UNavigationSystemV1* const nav_sys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (nav_sys->GetRandomPointInNavigableRadius(targetLocation, searchRadius, location, nullptr))
	{
		controller->GetBlackboard()->SetValueAsVector(bb_keys::target_location, location.Location);
	}

	Cast<AEnemy>(controller->GetPawn())->UpdateState(EnemyState::Investigating); // Updates enemy state

	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}


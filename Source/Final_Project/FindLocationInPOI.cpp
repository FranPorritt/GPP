// Fill out your copyright notice in the Description page of Project Settings.


#include "FindLocationInPOI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Enemy_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "PointOfInterest.h"
#include "Engine/TargetPoint.h"

UFindLocationInPOI::UFindLocationInPOI(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Find location in POI");
}

EBTNodeResult::Type UFindLocationInPOI::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	// Get AI Controller
	auto const controller = Cast<AEnemy_AIController>(owner_comp.GetAIOwner());	
	AActor* const POITarget = controller->GetActiveTarget();
	if (POITarget)
	{
		FVector const POILocation = POITarget->GetActorLocation();

		FNavLocation location;

		// Get nav system and generate a random location on NavMesh
		UNavigationSystemV1* const nav_sys = UNavigationSystemV1::GetCurrent(GetWorld());
		if (nav_sys->GetRandomPointInNavigableRadius(POILocation, searchRadius, location, nullptr))
		{
			controller->GetBlackboard()->SetValueAsVector("TargetLocation", location.Location);
		}

		//controller->UpdatePOIPriorities();

		FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	FinishLatentTask(owner_comp, EBTNodeResult::Failed);
	return EBTNodeResult::Failed;
}

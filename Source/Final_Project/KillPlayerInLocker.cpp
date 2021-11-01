// Fill out your copyright notice in the Description page of Project Settings.


#include "KillPlayerInLocker.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Enemy_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "BlackboardKeys.h"
#include "Final_ProjectCharacter.h"

UKillPlayerInLocker::UKillPlayerInLocker(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Kill player in locker");
}

EBTNodeResult::Type UKillPlayerInLocker::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	// Get AI Controller and Player
	auto const controller = Cast<AEnemy_AIController>(owner_comp.GetAIOwner());
	AActor* locker = Cast<AFinal_ProjectCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->GetCurrentLocker();

	if (locker)
	{
		FVector lockerLocation = locker->GetActorLocation();

		FNavLocation location;
		// Get nav system and generate a random location on NavMesh
		UNavigationSystemV1* const nav_sys = UNavigationSystemV1::GetCurrent(GetWorld());
		if (nav_sys->GetRandomPointInNavigableRadius(lockerLocation, searchRadius, location, nullptr))
		{
			controller->SetCurrentLocker(locker);
			controller->GetBlackboard()->SetValueAsVector(bb_keys::target_location, location.Location);
		}

		FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	FinishLatentTask(owner_comp, EBTNodeResult::Failed);
	return EBTNodeResult::Failed;
}

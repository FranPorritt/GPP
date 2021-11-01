// Fill out your copyright notice in the Description page of Project Settings.


#include "StalkingPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Enemy_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "BlackboardKeys.h"
#include "Enemy.h"
#include "Final_ProjectCharacter.h"

UStalkingPlayer::UStalkingPlayer(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Stalking player");
}

EBTNodeResult::Type UStalkingPlayer::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	// Get AI Controller and Player
	auto const controller = Cast<AEnemy_AIController>(owner_comp.GetAIOwner());
	AActor* flashlight = controller->GetFlashlight();

	if (flashlight)
	{
		if (Cast<AFinal_ProjectCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->GetHiddenStatus()) // If player is hiding and enemy sees light
		{
			controller->GetBlackboard()->SetValueAsBool(bb_keys::locker_flashlight, true);
		}

		Cast<AEnemy>(controller->GetPawn())->UpdateState(EnemyState::Stalking);
		FVector lightLocation = flashlight->GetActorLocation();

		FNavLocation location;
		// Get nav system and generate a random location on NavMesh
		UNavigationSystemV1* const nav_sys = UNavigationSystemV1::GetCurrent(GetWorld());
		if (nav_sys->GetRandomPointInNavigableRadius(lightLocation, searchRadius, location, nullptr))
		{
			controller->GetBlackboard()->SetValueAsVector(bb_keys::target_location, location.Location);
			controller->GetBlackboard()->SetValueAsBool(bb_keys::is_stalking, false);
		}

		FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	FinishLatentTask(owner_comp, EBTNodeResult::Failed);
	return EBTNodeResult::Failed;
}

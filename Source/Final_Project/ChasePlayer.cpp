// Fill out your copyright notice in the Description page of Project Settings.


#include "ChasePlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Enemy_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BlackboardKeys.h"

UChasePlayer::UChasePlayer(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Chase Player");
}

EBTNodeResult::Type UChasePlayer::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	// Get TargetLocation from bb via AI controller
	auto const controller = Cast<AEnemy_AIController>(owner_comp.GetAIOwner());
	FVector const playerLocation = controller->GetBlackboard()->GetValueAsVector(bb_keys::target_location);

	// Move to players location
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, playerLocation);

	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

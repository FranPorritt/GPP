// Fill out your copyright notice in the Description page of Project Settings.


#include "FinishInvestigating.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "BlackboardKeys.h"
#include "Enemy.h"

UFinishInvestigating::UFinishInvestigating(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("End Investigation");
}

EBTNodeResult::Type UFinishInvestigating::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	// Get AI Controller
	auto const controller = Cast<AEnemy_AIController>(owner_comp.GetAIOwner());
	
	controller->GetBlackboard()->SetValueAsBool(bb_keys::is_investigating, false);
	Cast<AEnemy>(controller->GetPawn())->UpdateState(EnemyState::Searching); // Updates enemy state

	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}


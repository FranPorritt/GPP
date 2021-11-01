// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckLockerDecision.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Enemy_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Director.h"
#include "BlackboardKeys.h"

UCheckLockerDecision::UCheckLockerDecision(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Decide if searching locker");
}

EBTNodeResult::Type UCheckLockerDecision::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	auto const controller = Cast<AEnemy_AIController>(owner_comp.GetAIOwner());
	director = Cast<ADirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ADirector::StaticClass()));

	if (director->GetLockerChance())
	{
		controller->GetBlackboard()->SetValueAsBool(bb_keys::should_search_locker, true);
		UE_LOG(LogTemp, Error, TEXT("searching locker"));
	}
	else
	{
		controller->GetBlackboard()->SetValueAsBool(bb_keys::should_search_locker, false);
		UE_LOG(LogTemp, Error, TEXT("NOT searching locker"));
	}

	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

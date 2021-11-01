// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayEnemySound.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy_AIController.h"

UPlayEnemySound::UPlayEnemySound(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Play Enemy Sound");
}

EBTNodeResult::Type UPlayEnemySound::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	// Get AI Controller
	auto const controller = Cast<AEnemy_AIController>(owner_comp.GetAIOwner());
	
	if (controller)
	{
		controller->PlayEnemySound(FName(TEXT("Growl")));

		FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	FinishLatentTask(owner_comp, EBTNodeResult::Failed);
	return EBTNodeResult::Failed;
}

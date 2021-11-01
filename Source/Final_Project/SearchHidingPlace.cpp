// Fill out your copyright notice in the Description page of Project Settings.


#include "SearchHidingPlace.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "Enemy_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "BlackboardKeys.h"
#include "HidingPlace.h"
#include "Final_ProjectGameMode.h"


USearchHidingPlace::USearchHidingPlace(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Search locker");
}

EBTNodeResult::Type USearchHidingPlace::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	// Get AI Controller and Player
	auto const controller = Cast<AEnemy_AIController>(owner_comp.GetAIOwner());
	AActor* locker = controller->GetCurrentLocker();

	if (locker)
	{
		if (Cast<AHidingPlace>(locker)->GetHidingStatus()) // Player in locker
		{
			controller->LockerQTE(); // QTE
		}
		else
		{
			controller->PlayEnemySound(FName(TEXT("Locker")));
			controller->GetBlackboard()->SetValueAsBool(bb_keys::heard_locker, false); // If heard locker and checked but player no longer there and wasn't spotted
		}

		FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	FinishLatentTask(owner_comp, EBTNodeResult::Failed);
	return EBTNodeResult::Failed;
}
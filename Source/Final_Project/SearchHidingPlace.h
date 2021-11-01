// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Enemy_AIController.h"
#include "Final_ProjectCharacter.h"
#include "SearchHidingPlace.generated.h"

/**
 * 
 */
UCLASS()
class FINAL_PROJECT_API USearchHidingPlace : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
		float searchRadius = 50.0f;
public:
	USearchHidingPlace(FObjectInitializer const& object_initializer);
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory) override;
};

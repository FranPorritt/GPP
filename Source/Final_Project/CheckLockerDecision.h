// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "CheckLockerDecision.generated.h"

/**
 * 
 */
UCLASS()
class FINAL_PROJECT_API UCheckLockerDecision : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
protected:
	class ADirector* director;
public:
	UCheckLockerDecision(FObjectInitializer const& object_initializer);
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory) override;
};

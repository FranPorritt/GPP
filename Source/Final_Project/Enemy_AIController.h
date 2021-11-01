// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "Containers/Map.h"
#include "Enemy_AIController.generated.h"

/**
 * 
 */

USTRUCT()
struct FAIBehaviours
{
    GENERATED_BODY()

    bool SearchLockersUnlocked = false;      // 0
    bool ThrowableKnowledgeUnlocked = false;  // 1   -- Knows a thrown object is a distraction
};

UCLASS()
class FINAL_PROJECT_API AEnemy_AIController : public AAIController
{
    GENERATED_BODY()

protected:
    class ADirector* director;

    AActor* currentTargetPOI = nullptr;
    UPROPERTY(BlueprintReadWrite)
    AActor* currentTargetLocker = nullptr;
    AActor* flashlight = nullptr;

    FAIBehaviours behaviours;
    FTimerHandle startBehaviourTimer;

    float hearingRange = 5000.0f;
    float sightRadius = 2500.0f;

    UPROPERTY(BlueprintReadWrite)
    bool investigatingPlayerNoise = false;

    void CheckDistance(AActor* actor); // Checks if the player is close enough to kill
    void CheckNoise(AActor* actor);
    void SpawnNoisePOI(AActor* actor);
    void EndStart(); // Enemy travels to random pos first then begins searching -- function prevents it from repeating starting behaviour
    void SortArray(TArray<AActor*> array);
    UFUNCTION(BlueprintImplementableEvent)
    void ResetPlayerInvestigate();

private:
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
    class UBehaviorTreeComponent* behaviorTreeComponent;
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
    class UBehaviorTree* behaviorTree;
    class UBlackboardComponent* blackboard;
  
    class UAIPerceptionComponent* PerceptionComponent;
    class UAISenseConfig_Sight* sightConfig;
    class UAISenseConfig_Hearing* hearingConfig;

    UFUNCTION()
    void SetupPerceptionSystem();
    UFUNCTION()
    void OnTargetDetected(AActor* actor, FAIStimulus const stimulus);
public:
    AEnemy_AIController(FObjectInitializer const& object_initializer = FObjectInitializer::Get());

    void BeginPlay() override;
    void OnPossess(APawn* const pawn) override;

    class UBlackboardComponent* GetBlackboard() const;

    void UnlockBehaviour(int key, bool status);
    bool GetBehaviourStatus(int key); // Returns status of behaviour associated with key

    AActor* ChooseTargetPOI();
    AActor* GetActiveTarget() { return currentTargetPOI; };
    void SetActiveTarget(AActor* actor) { currentTargetPOI = actor; };

    AActor* FindLocker();
    AActor* GetCurrentLocker() { return currentTargetLocker; };
    void SetCurrentLocker(AActor* lockerArg) { currentTargetLocker = lockerArg; };
    void LockerQTE();
    void CheckQTESuccess();

    AActor* GetFlashlight() { return flashlight; };

    AActor* currentThrowableObject;

    UFUNCTION(BlueprintImplementableEvent)
    void TESTCallStim(AActor* actor);
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void PlayEnemySound(FName soundType);

    void SetHearingRange(float* hearingRangeArg) { hearingRange = *hearingRangeArg; };
    void SetSightRadius(float* sightRadiusArg) { sightRadius = *sightRadiusArg; };
};

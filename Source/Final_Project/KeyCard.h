// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "KeyCard.generated.h"

UCLASS()
class FINAL_PROJECT_API AKeyCard : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* KeyCardMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Trigger Box")
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere)
		class AActor* player;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintImplementableEvent)
	void PickedUp();
	
public:	
	// Sets default values for this actor's properties
	AKeyCard();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

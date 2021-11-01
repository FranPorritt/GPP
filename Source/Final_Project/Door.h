// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class FINAL_PROJECT_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UStaticMeshComponent* DoorMesh;
	UPROPERTY(BlueprintReadWrite)
	FVector initialPos;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void Open();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void Close();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

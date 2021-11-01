// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "SpawnableObjects.generated.h"

UCLASS()
class FINAL_PROJECT_API ASpawnableObjects : public AActor
{
	GENERATED_BODY()	

protected:

	UPROPERTY(BlueprintReadWrite)
	TArray<UStaticMesh*> meshes;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	ASpawnableObjects();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

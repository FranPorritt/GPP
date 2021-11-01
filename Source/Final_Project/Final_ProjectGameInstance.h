// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include <Final_Project\Director.h>
#include "Final_ProjectGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FINAL_PROJECT_API UFinal_ProjectGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    FDirectorData directorData;
	UPROPERTY(BlueprintReadWrite)
	bool checkpointReached = false;
};

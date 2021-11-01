// Fill out your copyright notice in the Description page of Project Settings.

#include "KeyCard.h"
#include "Final_ProjectCharacter.h"

// Sets default values
AKeyCard::AKeyCard()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	KeyCardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Keycard Mesh"));
	KeyCardMesh->SetupAttachment(RootComponent);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box"));
	TriggerBox->InitBoxExtent(FVector(10.0f, 6.0f, 1.0f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetupAttachment(KeyCardMesh);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AKeyCard::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AKeyCard::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKeyCard::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherActor == player)
	{
		Cast<AFinal_ProjectCharacter>(player)->GetKeyCard();
		PickedUp();
		//Destroy(); // Moved to bp because it was preventing sound playing
	}
}

// Called every frame
void AKeyCard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Copyright Epic Games, Inc. All Rights Reserved.

#include "Final_ProjectProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Perception/AISense_Hearing.h"
#include "PointOfInterest.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Enemy_AIController.h"



AFinal_ProjectProjectile::AFinal_ProjectProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AFinal_ProjectProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	//CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	//CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 10.0f;

	Tags.Add("Throwable");
}

void AFinal_ProjectProjectile::BeginPlay()
{
	Super::BeginPlay();

	startingPos = GetActorLocation();
}

void AFinal_ProjectProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}

	if (!spawnedPOIOnHit)
	{
		FVector const location = GetActorLocation();
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), location, 1.0f, this, 0.0f, "Noise");

		spawnedPOIOnHit = true;
	}
}
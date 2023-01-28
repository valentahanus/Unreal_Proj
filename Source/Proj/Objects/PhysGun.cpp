// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysGun.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Proj.h"


// Sets default values
APhysGun::APhysGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APhysGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APhysGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APhysGun::Fire()
{
	Super::Fire();

	FCollisionQueryParams QueryParams = FCollisionQueryParams();
	QueryParams.bReturnPhysicalMaterial = false;
	QueryParams.bIgnoreTouches = true;
	
	// TODO Add ignore tags here
	
	QueryParams.AddIgnoredActor(this);
	
	FHitResult TraceResult;
	FRotator CameraRotation = Camera->GetComponentRotation();
	FVector CameraLocation = Camera->GetComponentLocation();

	double PickupRange = 200;

	FVector CameraDirection = CameraRotation.RotateVector(FVector(1, 0, 0));
	FVector TraceOffset = CameraDirection * PickupRange;
	FVector TraceLocation = CameraLocation + TraceOffset;
	
	bool ValidHit = GetWorld()->LineTraceSingleByChannel(TraceResult, CameraLocation, TraceLocation, PickUpChannel, QueryParams);

	// Action called if it hits something
	if (!TraceResult.GetActor())
	{
		return;
	}

	//TraceResult.GetActor()->SetActorLocation(TraceLocation);
	
	PhysicsConstraint->SetConstrainedComponents(
		Cast<UPrimitiveComponent>(TraceResult.GetActor()->GetRootComponent()),
		FName(),
		Cast<UPrimitiveComponent>(ConstraintDummy),
		FName()
	);
}


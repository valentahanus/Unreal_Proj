// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysGun.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Proj.h"
#include "DrawDebugHelpers.h"

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

GunEffectVariant APhysGun::Fire(FRotator CharacterRotation)
{
	Super::Fire(CharacterRotation);

	if (IsActive())
	{
		Drop(CharacterRotation);
	}
	else
	{
		PickUp(CharacterRotation);
	}

	GunEffectVariant Effect{};
	UPhysGunEffect* PhysGunEffect = NewObject<UPhysGunEffect>();
	PhysGunEffect->bIsActive = IsActive();

	Effect.Set<UPhysGunEffect*>(PhysGunEffect);

	return Effect;
}

void APhysGun::TriggerClientVFX(FRotator CharacterRotation, GunEffectVariant Variant)
{
	ClientPlayVFX(CharacterRotation, GetGunEffectInsideVariant(Variant));
}

void APhysGun::Drop(FRotator CharacterRotation)
{
	PhysicsConstraint->BreakConstraint();

	/*LOG("Droped")*/
}

void APhysGun::PickUp(FRotator CharacterRotation)
{
	FCollisionQueryParams QueryParams = FCollisionQueryParams();
	QueryParams.bReturnPhysicalMaterial = false;
	QueryParams.bIgnoreTouches = true;
	
	// TODO Add ignore tags here
	
	QueryParams.AddIgnoredActor(this);
	
	FHitResult TraceResult;
	FRotator ShootRotation = CharacterRotation;
	FVector ShootLocation = GetActorLocation();

	double PickupRange = 200;

	FVector ShootDirection = ShootRotation.RotateVector(FVector(1, 0, 0));
	FVector TraceOffset = ShootDirection * PickupRange;
	FVector TraceLocation = ShootLocation + TraceOffset;
	
	bool ValidHit = GetWorld()->LineTraceSingleByChannel(TraceResult, ShootLocation, TraceLocation, PickUpChannel, QueryParams);

	// Action called if it hits something
	if (!TraceResult.GetActor())
	{
		/*DrawDebugLine
		(
			this->GetWorld(),
			ShootLocation,
			TraceLocation,
			FColor::Cyan,
			true,
			1,
			2,
			4
		);*/
		return;
	}

	//TraceResult.GetActor()->SetActorLocation(TraceLocation);

	ENSURE_NOTNULL(PhysicsConstraint)
	ENSURE_NOTNULL(ConstraintDummy)
	
	PhysicsConstraint->SetConstrainedComponents(
		Cast<UPrimitiveComponent>(TraceResult.GetActor()->GetRootComponent()),
		FName(),
		Cast<UPrimitiveComponent>(ConstraintDummy),
		FName()
	);
	/*LOG("Picked up")*/
}

bool APhysGun::IsActive()
{
	return PhysicsConstraint != nullptr && PhysicsConstraint->ConstraintInstance.IsValidConstraintInstance();
}

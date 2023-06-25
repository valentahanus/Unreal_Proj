// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunBase.h"
#include "GunVFX.h"
#include "PhysGun.generated.h"

class UPhysicsConstraintComponent;
UCLASS()
class PROJ_API APhysGun : public AGunBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APhysGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Fire override
	virtual GunEffectVariant Fire(FRotator CharacterRotation) override;

	virtual void TriggerClientVFX(FRotator CharacterRotation, GunEffectVariant Variant) override;

private:

	void Drop(FRotator CharacterRotation);

	void PickUp(FRotator CharacterRotation);

	bool IsActive();

public:

	void SetPhysicsConstraint(UPhysicsConstraintComponent* InPhysicsConstraint) { PhysicsConstraint = InPhysicsConstraint; }
	void SetConstraintDummy(UStaticMeshComponent* InConstraintDummy) { ConstraintDummy = InConstraintDummy; }

protected:

	UPROPERTY()
	UPhysicsConstraintComponent* PhysicsConstraint;

	UPROPERTY()
	UStaticMeshComponent* ConstraintDummy;
};

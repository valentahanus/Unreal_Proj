// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Proj.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UVisualChildActorComponent;
class UWeaponComponent;
class UCameraComponent;
class UPhysicsConstraintComponent;
class UPlayerReplicationComponent;

UCLASS()
class PROJ_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	
	// These thing are visible in blueprints
	UPROPERTY(EditDefaultsOnly)
	FVector2D PitchLimits;

	// Default player speed
	UPROPERTY(EditDefaultsOnly)
	int32 PlayerSpeed;

	UPROPERTY(EditDefaultsOnly)
	int32 CharacterRotationSpeed;

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UPlayerReplicationComponent* PlayerReplicationComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UWeaponComponent* WeaponComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* ItemAnchor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UVisualChildActorComponent* PhysGunAnchor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UVisualChildActorComponent* PistolAnchor;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* ConstraintDummy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UPhysicsConstraintComponent* PhysicsConstraint;

public: // Picking up
	
	void Fire();

	void Drop();

private: // Movement

	// Speeds up player speed 
	void Run();

	// Rotates Character left or right
	void RotateCharacterHorizontal(float Axis);

	// Rotates camera up or down
	void RotateCameraVertical(float Axis);

	// Moves character forward or backward
	void MoveCharacterForward(float Axis);

	// Moves character left or right
	void MoveCharacterRight(float Axis);

public: // Getters

	UPhysicsConstraintComponent* GetPhysicsConstraint() { return PhysicsConstraint; }
	UStaticMeshComponent* GetConstraintDummy() { return ConstraintDummy; }
};
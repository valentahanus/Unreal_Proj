// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class UPhysicsConstraintComponent;

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
	
	UPROPERTY(EditDefaultsOnly)
	int32 PlayerSpeed;

	UPROPERTY(EditDefaultsOnly)
	int32 CharacterRotationSpeed;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UPhysicsConstraintComponent* PhysicsConstraint;

private: // Picking Up

	void PickUp();

	void Drop();

private: // Movement

	// Speeds up player speed 
	void Run();

	// Rotates camera left or right
	void RotateCharacterHorizontal(float Axis);

	// Rotates camera up or down
	void RotateCameraVertical(float Axis);

	// Moves character forward or backward
	void MoveCharacterForward(float Axis);

	// Moves character left or right
	void MoveCharacterRight(float Axis);
	
	
};

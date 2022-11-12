// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Proj.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class UPhysicsConstraintComponent;

DECLARE_DELEGATE_OneParam(FOnWeaponSelectedDelegate, uint8);

UCLASS()
class PROJ_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	//
	FOnWeaponSelectedDelegate OnWeaponSelected;

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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UPhysicsConstraintComponent* PhysicsConstraint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* ConstraintDummy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* ItemAnchor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* PhysGun;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Pistol;

protected: // Replication

	UPROPERTY(Replicated, ReplicatedUsing=OnRep_SelectedWeapon)
	uint8 SelectedWeapon;

	UFUNCTION()
	void OnRep_SelectedWeapon();
	
protected: // VFX

	UFUNCTION(BlueprintImplementableEvent)
	void PlayPickUpVFX();
	
private: // Picking Up

	void PickUp();

	void Drop();

	void RequestFirstSlotSelected() { Server_RequestWeaponChange(0); }
	void RequestSecondSlotSelected() { Server_RequestWeaponChange(1); }

	void FirstSlotSelected();
	void SecondSlotSelected();

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

public: // Networking

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private: // Client -> Server

	UFUNCTION(Server, Unreliable)
	void Server_SendRotation(double Yaw, double Pitch);

	UFUNCTION(Server, Reliable)
	void Server_RequestWeaponChange(uint8 WeaponIndex);

private: // Cheats

	UFUNCTION(Exec)
	void SelectWeapon(uint8 WeaponIndex);
};
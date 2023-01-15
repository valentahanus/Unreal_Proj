// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerComponent.h"
#include "WeaponComponent.generated.h"

UENUM(BlueprintType)
enum class EWeapon : uint8
{
    None = 0,
    Pistol,
    PhysGun
};

DECLARE_DELEGATE_OneParam(FOnWeaponSelectedDelegate, EWeapon);

class UCameraComponent;
class APhysGun;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJ_API UWeaponComponent : public UPlayerComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UWeaponComponent();

	FOnWeaponSelectedDelegate OnWeaponSelected;

protected: // Called when the game starts
	
	virtual void SetupPlayerComponent(APlayerCharacter* InOwningCharacter) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected: // Replication

	UPROPERTY(Replicated, ReplicatedUsing=OnRep_SelectedWeapon)
	EWeapon SelectedWeapon;

	UFUNCTION()
	void OnRep_SelectedWeapon();

public: // Weapons

	EWeapon GetSelectedWeapon();

protected: // VFX

	UFUNCTION(BlueprintImplementableEvent)
	void PlayPickUpVFX();

public: // Switching

	void RequestFirstSlotSelected() { Server_RequestWeaponChange(0); }
	void RequestSecondSlotSelected() { Server_RequestWeaponChange(1); }

private: // Switching

	void FirstSlotSelected();
	void SecondSlotSelected();

private: // Client -> Server

	UFUNCTION(Server, Reliable)
	void Server_RequestWeaponChange(uint8 WeaponIndex);

private: // Server -> Client

	UFUNCTION(Client, Reliable)
	void Client_ForceSelectedWeapon(uint8 WeaponIndex);

private: // Cheats

	UFUNCTION(Exec)
	void SelectWeapon(uint8 WeaponIndex);

public: // Actions

	void Fire();

private: // Guns

	UPROPERTY()
	APhysGun* PhysGun;

public:

	void SetGun(APhysGun* InGun);
};

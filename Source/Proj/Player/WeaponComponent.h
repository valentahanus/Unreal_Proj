// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerComponent.h"
#include "Proj.h"
#include "WeaponComponent.generated.h"

class AGunBase;
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
class UVisualChildActorComponent;

USTRUCT()
struct FWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY()
	AGunBase* HeldGun = nullptr;
	
	UPROPERTY()
	AGunBase* VisualGun = nullptr;

	UPROPERTY()
	USceneComponent* HeldGunOwner = nullptr;
};

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
	EWeapon SelectedWeaponIndex;

	UFUNCTION()
	void OnRep_SelectedWeapon();

public: // Weapons

	EWeapon GetSelectedWeaponIndex();

protected: // VFX

	UFUNCTION(BlueprintImplementableEvent)
	void PlayPickUpVFX();

public: // Switching

	void RequestFirstSlotSelected() { Server_RequestWeaponChange(static_cast<uint8>(EWeapon::Pistol)); }
	void RequestSecondSlotSelected() { Server_RequestWeaponChange(static_cast<uint8>(EWeapon::PhysGun)); }

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
	TArray<FWeaponInfo> HeldGuns;

	TArray<FRotator> LastGunRotation;

	int32 index = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 WobblyGunBufferSize;

	UPROPERTY(EditDefaultsOnly)
	float WobblyGunInterpolationStrength;

public: // Locally controlled client only

	void SpawnGun(EWeapon WeaponIndex, UVisualChildActorComponent* VisualComponent);
	
	FWeaponInfo& GetWeaponInfo(EWeapon WeaponType);
};

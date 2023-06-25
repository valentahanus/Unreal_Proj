// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerComponent.h"
#include "Proj.h"
#include "Objects/GunVFX.h"
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

public: // local client only

	UPROPERTY()
	AGunBase* HeldGun = nullptr;

	UPROPERTY()
	USceneComponent* HeldGunOwner = nullptr;

public: // non local only
	
	UPROPERTY()
	AGunBase* VisualGun = nullptr;

	UPROPERTY()
	USceneComponent* VisualGunOwner = nullptr;
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

private: // Client -> Server

	UFUNCTION(Server, Reliable)
	void Server_RequestWeaponChange(uint8 WeaponIndex);

private: // Client -> Server
	
	UFUNCTION(Server, Reliable)
	void Server_Fire(FRotator CharacterRotation);

public:
	
	void Fire(FRotator CharacterRotation);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCast_OnFired(const TArray<uint8>& EffectBuffer);
	
private: // Server -> Client

private: // Cheats

	UFUNCTION(Exec)
	void SelectWeapon(uint8 WeaponIndex);

private: // Guns

	UPROPERTY()
	TArray<FWeaponInfo> WeaponArray;

	TArray<FRotator> LastGunRotation;

	int32 index = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 WobblyGunBufferSize;

	UPROPERTY(EditDefaultsOnly)
	float WobblyGunInterpolationStrength;

public: // Locally controlled client only

	void SpawnGun_ServerOrClient(EWeapon WeaponIndex, UVisualChildActorComponent* VisualComponent);
	
	FWeaponInfo& GetWeaponInfo(EWeapon WeaponType);

private: // Simulated proxy only

	void SpawnGun_SimulatedProxy(EWeapon WeaponIndex, UVisualChildActorComponent* VisualComponent);
};

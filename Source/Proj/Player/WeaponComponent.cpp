// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/WeaponComponent.h"

#include <Objects/PhysGun.h>

#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

#include "DrawDebugHelpers.h"
#include "PlayerCharacter.h"
#include "Components/VisualChildActorComponent.h"

#include <cassert>

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	WobblyGunBufferSize = 20;
	WobblyGunInterpolationStrength = 0.5f;

	// ...
}


// Called when the game starts
void UWeaponComponent::SetupPlayerComponent(APlayerCharacter* InOwningCharacter)
{
	Super::SetupPlayerComponent(InOwningCharacter);
	
	WeaponArray.SetNum(StaticEnum<EWeapon>()->GetMaxEnumValue());

	if (OwningCharacter->IsLocallyControlled() || OwningCharacter->GetLocalRole() == ROLE_Authority)
	{
		SpawnGun_ServerOrClient(EWeapon::PhysGun, OwningCharacter->PhysGunAnchor);
		SpawnGun_ServerOrClient(EWeapon::Pistol, OwningCharacter->PistolAnchor);

		APhysGun* PhysGun = static_cast<APhysGun*>(GetWeaponInfo(EWeapon::PhysGun).HeldGun);
                                                                 	
        PhysGun->SetPhysicsConstraint(OwningCharacter->GetPhysicsConstraint());
        PhysGun->SetConstraintDummy(OwningCharacter->GetConstraintDummy());
        
        LastGunRotation.Init(OwningCharacter->PhysGunAnchor->GetComponentRotation(), WobblyGunBufferSize);
	}
	else
	{
		SpawnGun_SimulatedProxy(EWeapon::PhysGun, OwningCharacter->MultiplayerPhysGunAnchor);	
		SpawnGun_SimulatedProxy(EWeapon::Pistol, OwningCharacter->MultiplayerPistolAnchor);	
	}

	if (OwningCharacter->GetLocalRole() == ENetRole::ROLE_Authority)
	{
		Server_RequestWeaponChange_Implementation(static_cast<uint8>(EWeapon::None));
	}
}

void UWeaponComponent::SpawnGun_ServerOrClient(EWeapon WeaponIndex, UVisualChildActorComponent* VisualComponent)
{
	ENSURE_TRUE(OwningCharacter->IsLocallyControlled() || OwningCharacter->GetLocalRole() == ROLE_Authority)
	
	ENSURE_TRUE(WeaponIndex != EWeapon::None);
	ENSURE_NOTNULL(VisualComponent);

	AGunBase* Spawned = GetWorld()->SpawnActor<AGunBase>(VisualComponent->GetChildActorClass());
	ENSURE_NOTNULL(Spawned)

	Spawned->AttachToComponent(VisualComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

	GetWeaponInfo(WeaponIndex).HeldGun = Spawned;
	GetWeaponInfo(WeaponIndex).HeldGunOwner = VisualComponent;
}

void UWeaponComponent::SpawnGun_SimulatedProxy(EWeapon WeaponIndex, UVisualChildActorComponent* VisualComponent)
{
	ENSURE_TRUE(!OwningCharacter->IsLocallyControlled())
	
	ENSURE_TRUE(WeaponIndex != EWeapon::None);
	ENSURE_NOTNULL(VisualComponent);

	AGunBase* Spawned = GetWorld()->SpawnActor<AGunBase>(VisualComponent->GetChildActorClass());
	ENSURE_NOTNULL(Spawned)

	Spawned->AttachToComponent(VisualComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

	GetWeaponInfo(WeaponIndex).VisualGun = Spawned;
	GetWeaponInfo(WeaponIndex).VisualGunOwner = VisualComponent;
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, SelectedWeaponIndex);
}

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Wobbly gun
	if (OwningCharacter->IsLocallyControlled() && SelectedWeaponIndex != EWeapon::None)
	{
		USceneComponent* GunOwner = GetWeaponInfo(SelectedWeaponIndex).HeldGunOwner;
		AGunBase* Gun = GetWeaponInfo(SelectedWeaponIndex).HeldGun;
		
        ENSURE_NOTNULL(Gun)
        ENSURE_NOTNULL(GunOwner);
        
        FRotator CurrentRotation = GunOwner->GetComponentRotation();
        Gun->SetActorRotation(FMath::Lerp(LastGunRotation[index], Gun->GetActorRotation(), WobblyGunInterpolationStrength));
        LastGunRotation[index] = CurrentRotation;
    
        index++;
        index %= WobblyGunBufferSize;
	}
}

void UWeaponComponent::OnRep_SelectedWeapon()
{
	uint8 Index = 1;
	for (FWeaponInfo& Weapon : WeaponArray)
	{
		if (Weapon.HeldGunOwner != nullptr)
		{
			Weapon.HeldGunOwner->SetHiddenInGame(Index != static_cast<uint8>(SelectedWeaponIndex), true);
		}

		if (Weapon.VisualGunOwner != nullptr)
		{
			Weapon.VisualGunOwner->SetHiddenInGame(Index != static_cast<uint8>(SelectedWeaponIndex), true);
		}
		
		Index++;
	}
	OnWeaponSelected.ExecuteIfBound(SelectedWeaponIndex);
}

EWeapon UWeaponComponent::GetSelectedWeaponIndex()
{
	return SelectedWeaponIndex;
}

void UWeaponComponent::Server_RequestWeaponChange_Implementation(uint8 WeaponIndex)
{
	SelectedWeaponIndex = static_cast<EWeapon>(WeaponIndex);

	if (OwningCharacter->IsLocallyControlled())
	{
		OnRep_SelectedWeapon();
	}
}

void UWeaponComponent::SelectWeapon(uint8 WeaponIndex)
{
	Server_RequestWeaponChange(WeaponIndex);
}

void UWeaponComponent::Server_Fire_Implementation()
{
	if (SelectedWeaponIndex == EWeapon::None)
	{
		return;
	}

	ENSURE_NOTNULL(GetWeaponInfo(SelectedWeaponIndex).HeldGun);
	
	GetWeaponInfo(SelectedWeaponIndex).HeldGun->Fire();
}

FWeaponInfo& UWeaponComponent::GetWeaponInfo(EWeapon WeaponType)
{
	assert(HeldGuns.Num() > 0);
	ENSURE_TRUE(WeaponType != EWeapon::None, WeaponArray[0]);

	return WeaponArray[static_cast<int32>(WeaponType) - 1];
}

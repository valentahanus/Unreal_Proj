// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/WeaponComponent.h"

#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

#include "DrawDebugHelpers.h"
#include "PlayerCharacter.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	// ...
}


// Called when the game starts
void UWeaponComponent::SetupPlayerComponent(APlayerCharacter* InOwningCharacter)
{
	Super::SetupPlayerComponent(InOwningCharacter);

	// ...
	Client_ForceSelectedWeapon(0);
}


// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponComponent::OnRep_SelectedWeapon()
{
	switch (SelectedWeapon)
	{
	case 0:
		{
			FirstSlotSelected();
		}
		break;
		
	case 1:
		{
			SecondSlotSelected();
		}
		break;

	default:
		{
			check(false);
		}
	}
}

uint8 UWeaponComponent::GetSelectedWeapon()
{
	return SelectedWeapon;
}

void UWeaponComponent::FirstSlotSelected()
{

	OwningCharacter->Pistol->SetHiddenInGame(true, true);
	OwningCharacter->PhysGun->SetHiddenInGame(false, true);
	
	OnWeaponSelected.ExecuteIfBound(0);
}

void UWeaponComponent::SecondSlotSelected()
{
	OwningCharacter->PhysGun->SetHiddenInGame(true, true);
	OwningCharacter->Pistol->SetHiddenInGame(false, true);
	
	OnWeaponSelected.ExecuteIfBound(1);
}

void UWeaponComponent::Server_RequestWeaponChange_Implementation(uint8 WeaponIndex)
{
	if (WeaponIndex > 1)
	{
		return;
	}

	SelectedWeapon = WeaponIndex;

	if (OwningCharacter->IsLocallyControlled())
	{
		OnRep_SelectedWeapon();
	}
}

void UWeaponComponent::Client_ForceSelectedWeapon_Implementation(uint8 WeaponIndex)
{
	SelectedWeapon = WeaponIndex;

	OnRep_SelectedWeapon();
}

void UWeaponComponent::SelectWeapon(uint8 WeaponIndex)
{
	Server_RequestWeaponChange(WeaponIndex);
}

void UWeaponComponent::Fire()
{
	
}

void UWeaponComponent::SetGun(APhysGun* InGun)
{
	PhysGun = InGun;
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, SelectedWeapon);
}

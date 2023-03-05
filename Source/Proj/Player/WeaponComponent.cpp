// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/WeaponComponent.h"

#include <Objects/PhysGun.h>

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

	WobblyGunBufferSize = 20;
	WobblyGunInterpolationStrength = 0.5f;

	// ...
}


// Called when the game starts
void UWeaponComponent::SetupPlayerComponent(APlayerCharacter* InOwningCharacter)
{
	Super::SetupPlayerComponent(InOwningCharacter);

	// ...
	Client_ForceSelectedWeapon(static_cast<uint8>(EWeapon::None));
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	HeldGuns.SetNum(StaticEnum<EWeapon>()->GetMaxEnumValue());
	VisibleGuns.SetNum(StaticEnum<EWeapon>()->GetMaxEnumValue());
}


// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FRotator CurrentRotation = OwningCharacter->PhysGun->GetComponentRotation();
	
	PhysGun->SetActorRotation(FMath::Lerp(LastGunRotation[index], PhysGun->GetActorRotation(), WobblyGunInterpolationStrength));
	
	LastGunRotation[index] = CurrentRotation;

	index++;
	index %= WobblyGunBufferSize;
}

void UWeaponComponent::OnRep_SelectedWeapon()
{
	switch (SelectedWeapon)
	{
	case EWeapon::None:
		{
			
		}
		break;
		
	case EWeapon::Pistol:
		{
			FirstSlotSelected();
		}
		break;
		
	case EWeapon::PhysGun:
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

EWeapon UWeaponComponent::GetSelectedWeapon()
{
	return SelectedWeapon;
}

void UWeaponComponent::FirstSlotSelected()
{

	OwningCharacter->Pistol->SetHiddenInGame(false, true);
	OwningCharacter->PhysGun->SetHiddenInGame(true, true);
	
	OnWeaponSelected.ExecuteIfBound(EWeapon::Pistol);
}

void UWeaponComponent::SecondSlotSelected()
{
	OwningCharacter->PhysGun->SetHiddenInGame(false, true);
	OwningCharacter->Pistol->SetHiddenInGame(true, true);
	
	OnWeaponSelected.ExecuteIfBound(EWeapon::PhysGun);
}

void UWeaponComponent::Server_RequestWeaponChange_Implementation(uint8 WeaponIndex)
{
	SelectedWeapon = static_cast<EWeapon>(WeaponIndex);

	if (OwningCharacter->IsLocallyControlled())
	{
		OnRep_SelectedWeapon();
	}
}

void UWeaponComponent::Client_ForceSelectedWeapon_Implementation(uint8 WeaponIndex)
{
	SelectedWeapon = static_cast<EWeapon>(WeaponIndex);

	OnRep_SelectedWeapon();
}

void UWeaponComponent::SelectWeapon(uint8 WeaponIndex)
{
	Server_RequestWeaponChange(WeaponIndex);
}

void UWeaponComponent::Fire()
{
	switch (SelectedWeapon)
	{
	case EWeapon::PhysGun:
		PhysGun->Fire();
		break;

	case EWeapon::Pistol:
		break;

	case EWeapon::None:
		break;
		
	default:
		TEXT("Problem no weapon selected");
		break;
	}
}

void UWeaponComponent::SetGun(APhysGun* InGun)
{
	ENSURE_NOTNULL(InGun);
	
	PhysGun = InGun;

	PhysGun->SetPhysicsConstraint(OwningCharacter->GetPhysicsConstraint());
	PhysGun->SetConstraintDummy(OwningCharacter->GetConstraintDummy());
	
	LastGunRotation.Init(OwningCharacter->PhysGun->GetComponentRotation(), WobblyGunBufferSize);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, SelectedWeapon);
}

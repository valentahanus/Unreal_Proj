// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ProjPlayerController.h"

#include "PlayerCharacter.h"


void AProjPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GetPawn() && GetPawn()->IsA<APlayerCharacter>())
	{
		/*APlayerCharacter* Player = Cast<APlayerCharacter>(GetPawn());
		Player->OnWeaponSelected.BindUObject(this, &AProjPlayerController::OnPlayerWeaponSelected);*/
	}
}

void AProjPlayerController::OnPlayerWeaponSelected(uint8 WeaponIndex)
{
	if (GetHUD())
	{
		
	}
}

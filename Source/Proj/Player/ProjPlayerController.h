// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProjPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API AProjPlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	void OnPlayerWeaponSelected(uint8 WeaponIndex);
};
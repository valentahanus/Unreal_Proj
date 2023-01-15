// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProjPlayerController.generated.h"

enum class EWeapon : uint8;

/**
 * 
 */
UCLASS()
class PROJ_API AProjPlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	void OnPlayerWeaponSelected(EWeapon WeaponIndex);
};
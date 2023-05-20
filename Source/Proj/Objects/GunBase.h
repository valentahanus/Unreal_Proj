// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunVFX.h"
#include "GameFramework/Actor.h"
#include "GunBase.generated.h"

UCLASS()
class PROJ_API AGunBase : public AActor
{
	GENERATED_BODY()

public:

	virtual GunEffectVariant Fire(FRotator CharacterRotation);

	virtual void TriggerClientVFX(FRotator CharacterRotation);

protected: // VFX
	
	UFUNCTION(BlueprintImplementableEvent)
	void ClientPlayVFX(FRotator CharacterRotation, UGunEffectBase* Effect);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnFired();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunBase.generated.h"

UCLASS()
class PROJ_API AGunBase : public AActor
{
	GENERATED_BODY()

public:

	virtual void Fire(FRotator CharacterRotation);

public: // public vfx

	UFUNCTION(BlueprintImplementableEvent)
	void ClientPlayVFX(FRotator CharacterRotation);

protected: // VFX

	UFUNCTION(BlueprintImplementableEvent)
	void OnFired();
};

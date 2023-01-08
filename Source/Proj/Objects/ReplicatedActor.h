// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReplicatedActor.generated.h"

UCLASS()
class PROJ_API AReplicatedActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AReplicatedActor();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private: // Server -> all clients

	UFUNCTION(NetMulticast, Reliable)
	void SendTransform(FTransform Transform);
};

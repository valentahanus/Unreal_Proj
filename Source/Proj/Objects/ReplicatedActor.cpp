// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplicatedActor.h"


// Sets default values
AReplicatedActor::AReplicatedActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
}

// Called every frame
void AReplicatedActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetNetMode() == NM_DedicatedServer)
	{
		SendTransform(this->GetTransform());
	}
}

void AReplicatedActor::SendTransform_Implementation(FTransform Transform)
{
	SetActorTransform(Transform);
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerReplicationComponent.h"

#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UPlayerReplicationComponent::UPlayerReplicationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
	SetIsReplicatedByDefault(true);
	SetNetAddressable();

	// ...
}

void UPlayerReplicationComponent::SetupPlayerComponent(APlayerCharacter* InOwningCharacter)
{
	Super::SetupPlayerComponent(InOwningCharacter);
}

// Called every frame
void UPlayerReplicationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwningCharacter->IsLocallyControlled())
	{
		ViewPitch = OwningCharacter->Camera->GetComponentRotation().Pitch;
    
    	Server_SendRotation(OwningCharacter->GetActorRotation().Yaw, ViewPitch);	
	}
	// ...
}

void UPlayerReplicationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerReplicationComponent, ViewPitch);
}

void UPlayerReplicationComponent::Server_SendRotation_Implementation(double Yaw, double Pitch)
{
	OwningCharacter->SetActorRotation(FRotator( 0,Yaw,0));
	ViewPitch = Pitch;

	OwningCharacter->Camera->SetRelativeRotation(FRotator(Pitch, 0, 0));
}


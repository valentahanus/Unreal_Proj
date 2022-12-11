// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerComponent.h"
#include "Components/ActorComponent.h"
#include "PlayerReplicationComponent.generated.h"

class APlayerCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJ_API UPlayerReplicationComponent : public UPlayerComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerReplicationComponent();

protected:

	virtual void SetupPlayerComponent(APlayerCharacter* InOwningCharacter) override;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private: // Client -> Server

	UFUNCTION(Server, Unreliable)
	void Server_SendRotation(double Yaw, double Pitch);

protected: // Variables

	UPROPERTY(BlueprintReadOnly, Replicated)
	double ViewPitch;

public: // Networking

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

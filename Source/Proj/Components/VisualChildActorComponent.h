// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ChildActorComponent.h"
#include "VisualChildActorComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_API UVisualChildActorComponent : public UChildActorComponent
{
	GENERATED_BODY()

	virtual void CreateChildActor(TFunction<void(AActor*)> CustomizerFunc) override;
};

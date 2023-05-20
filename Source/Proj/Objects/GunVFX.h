#pragma once

#include "CoreMinimal.h"

#include "GunVFX.generated.h"

UCLASS()
class UGunEffectBase : public UObject
{
	GENERATED_BODY()
};

UCLASS(BlueprintType)
class UPhysGunEffect : public UGunEffectBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	bool bIsActive = false;
};

using GunEffectVariant = TVariant<bool, UPhysGunEffect*>;
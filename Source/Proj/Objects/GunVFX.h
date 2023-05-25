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

inline FArchive& operator<<(FArchive& Ar, GunEffectVariant& Variant) {
	if (Variant.GetIndex() == 0)
	{
		return Ar;
	}

	UGunEffectBase* Value = Visit([] (auto& Value)
	{
		return reinterpret_cast<UGunEffectBase*>(Value);
	}, Variant);
	return Ar << Value;
}

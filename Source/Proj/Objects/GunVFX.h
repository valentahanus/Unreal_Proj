#pragma once

#include "CoreMinimal.h"
#include "DiffUtils.h"
#include "Proj.h"

#include "GunVFX.generated.h"

UCLASS()
class UGunEffectBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void SerializeEffect(FArchive &Ar) {};
};

UCLASS(BlueprintType)
class UPhysGunEffect : public UGunEffectBase
{
	GENERATED_BODY()

	virtual void SerializeEffect(FArchive &Ar) override
	{
		Ar << bIsActive;
	}

public:

	UPROPERTY(BlueprintReadOnly)
	bool bIsActive = false;
};

using GunEffectVariant = TVariant<bool, UPhysGunEffect*>;

inline FArchive& operator<<(FArchive& Ar, GunEffectVariant& Variant)
{
        if (Ar.IsLoading()) {
	        SIZE_T Index;

        	Ar << Index;

        	Variant = GunEffectVariant{};

                switch (Index)
        	{
                case GunEffectVariant::IndexOfType<bool>():
			ENSURE_TRUE(false, Ar)
		break;
                	
                case GunEffectVariant::IndexOfType<UPhysGunEffect*>():
                	Variant.Set<UPhysGunEffect*>(NewObject<UPhysGunEffect>());
		break;
        	}
        }
        else
        {
	        if (Variant.GetIndex() == 0)
	        {
          		return Ar;
	        }
	        
	        UGunEffectBase* Value = Visit([] (auto& Value)
	        {
          		return reinterpret_cast<UGunEffectBase*>(Value);
	        }, Variant);
        	
        	SIZE_T Index = Variant.GetIndex();
        	
	        Ar << Index;
        	
	        Value->SerializeEffect(Ar);
	        
	       return Ar;
        }
}

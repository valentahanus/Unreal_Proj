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

/*
 * A helper function for populating a variant.
 * Will call itself until it either exhausts all indices, or finds the type.
 * 
 * If it finds the type, creates a new instance inside the variant.
 * Otherwise, triggers an assertion
 */
template<size_t i, class T, class ...Ts>
void CreateGunEffectByIndex(auto& Variant, int32 TargetIndex)
{
	if (i == TargetIndex)
	{
		Variant.template Emplace<T>( NewObject<T>() );
	}
	else if constexpr (sizeof...(Ts) == 0)
	{
		ENSURE_NO_ENTRY
	}
	else
	{
		CreateGunEffectByIndex<i + 1, Ts...>(Variant, TargetIndex);
	}
}

/*
 * Creates a new gun effect object inside the provided variant using the provided index
 * If the target index is 0, doesn't populate the variant at all
 */
template<class ...Ts>
void CreateGunEffectByIndex(TVariant<Ts...>& Variant, int32 TargetIndex)
{
	static_assert( sizeof...(Ts) > 0 );

	if (TargetIndex != 0)
	{
		return CreateGunEffectByIndex<1, Ts...>(Variant, TargetIndex);
	}
}

/*
 * Looks inside the provided variant, and returns the gun effect inside, cast to the base class
 * Everything other than the first item needs to inherit from UGunEffectBase
 */
inline UGunEffectBase* GetGunEffectInsideVariant(GunEffectVariant& Variant)
{
	// Index 0 doesn't contain a valid effect, abort!
	ENSURE_TRUE(Variant.GetIndex() != 0, nullptr);

	/*
	 * A templated lambda that generates a version for all possibilities inside the variant.
	 * Visit will call the template version that shares the type currently inside the variant.
	 *
	 * Example:
	 * Generate 2 versions, one for int, one for float. Will call the function with the same type that is currently inside the variant.
	 * Visit([](auto value) { print(value); }, TVariant<int, float>());		-> Results in 2 functions generated 
	 * [] (int value) { print(value); }										-> Called if there was an int inside
	 * [] (float value) { print(value); }									-> Called if there was a float inside
	 */
	return Visit([](auto item)->UGunEffectBase*
	{
		using TemplatedType = std::remove_all_extents_t<decltype(item)>;
		static constexpr int32 TypeIndex = GunEffectVariant::IndexOfType<TemplatedType>();

		// A compile time 'if', the template generation will generate a different code depending on the condition (we don't want to cast a bool to a gun effect base)
		if constexpr (TypeIndex != 0)
		{
			return static_cast<UGunEffectBase*>(item);
		}
		else
		{
			ENSURE_TRUE(false, nullptr)
		}
		
	}, Variant);
}

inline FArchive& operator<<(FArchive& Ar, GunEffectVariant& Variant)
{
        if (Ar.IsLoading()) {
                if (Ar.AtEnd()) {
			Variant = GunEffectVariant{};

                	return Ar;
                }

        	SIZE_T Index;
        	Ar << Index;

        	Variant = GunEffectVariant{};
        	CreateGunEffectByIndex(Variant, Index);

        	UGunEffectBase* GunEffectToLoad = GetGunEffectInsideVariant(Variant);
        	GunEffectToLoad->SerializeEffect(Ar);
        	
        	return Ar;
        }
        else
        {
	        if (Variant.GetIndex() == 0)
	        {
          		return Ar;
	        }
	        
	        UGunEffectBase* GunEffectToSave = GetGunEffectInsideVariant(Variant);
        	SIZE_T Index = Variant.GetIndex();
        	
	        Ar << Index;
	        GunEffectToSave->SerializeEffect(Ar);
	        
	       return Ar;
        }
}

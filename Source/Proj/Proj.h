// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

inline constexpr ECollisionChannel PickUpChannel = ECollisionChannel::ECC_GameTraceChannel2;

#define ENSURE_NOTNULL(x) \
if (x == nullptr) \
{ \
	static bool bTriggered = false; \
	if (bTriggered == false) \
	{ \
		UE_DEBUG_BREAK(); \
		bTriggered = true; \
	} \
	UE_LOG(LogTemp, Error, TEXT("%s is nullptr ,file:(%s), funcion:(%s)"), TEXT(#x), *FString(__FILE__), *FString(__FUNCTION__)) \
	return; \
}


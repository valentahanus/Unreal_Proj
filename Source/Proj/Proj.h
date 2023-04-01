// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

inline constexpr ECollisionChannel PickUpChannel = ECollisionChannel::ECC_GameTraceChannel2;

#define LOG(text, ...) UE_LOG(LogTemp, Warning, TEXT(text), __VA_ARGS__)

#define ENSURE_IMPL \
static bool bTriggered = false; \
if (bTriggered == false) \
{ \
    UE_DEBUG_BREAK(); \
    bTriggered = true; \
}

#define ENSURE_NOTNULL(x, ...) \
if ((x) == nullptr) \
{ \
	ENSURE_IMPL \
	UE_LOG(LogTemp, Error, TEXT("%s is nullptr ,file:(%s), funcion:(%s)"), TEXT(#x), *FString(__FILE__), *FString(__FUNCTION__)) \
	return __VA_ARGS__; \
}

#define ENSURE_TRUE(x, ...) \
if ((x) == false) \
{ \
	ENSURE_IMPL \
	UE_LOG(LogTemp, Error, TEXT("%s is false ,file:(%s), funcion:(%s)"), TEXT(#x), *FString(__FILE__), *FString(__FUNCTION__)) \
	return __VA_ARGS__; \
}

#define ENSURE_NO_ENTRY \
{ \
	ENSURE_IMPL \
	UE_LOG(LogTemp, Error, TEXT("ENSURE IS ANGY,file:(%s), funcion:(%s)"), *FString(__FILE__), *FString(__FUNCTION__)) \
	return; \
}

FString MyNetModeToString(ENetMode Mode);

FString NetRoleToString(ENetRole Role);

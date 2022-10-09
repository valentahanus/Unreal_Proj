// Copyright Epic Games, Inc. All Rights Reserved.


#include "ProjGameModeBase.h"

void AProjGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	//NEVER NEVER DO THIS!!!
	/*int a = 5;
	int* b = (int*)2;
	UE_LOG(LogTemp, Warning, TEXT("%i"), (*b));
	*b = 5;
	
	delete b;
	b = nullptr;*/
}
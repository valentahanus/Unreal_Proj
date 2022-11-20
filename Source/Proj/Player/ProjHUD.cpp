// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ProjHUD.h"

void AProjHUD::DrawHUD()
{
	for (int32 i = 0; i < DebugTestQueue.Num(); ++i)
	{
		DrawText(DebugTestQueue[i], FColor::Cyan, 0.0, 0.0);
	}

	DebugTestQueue.Reset();

	Super::DrawHUD();
}

void AProjHUD::AddDebugString(FString DebugString)
{
	DebugTestQueue.Add(DebugString);
}

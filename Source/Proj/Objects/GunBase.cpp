// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/GunBase.h"

#include "Proj.h"

void AGunBase::Fire(FRotator CharacterRotation)
{
	OnFired();
}

void AGunBase::TriggerClientVFX(FRotator CharacterRotation)
{
	ClientPlayVFX(CharacterRotation, nullptr);
}

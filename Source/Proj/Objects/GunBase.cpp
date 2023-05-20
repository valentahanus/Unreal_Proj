// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/GunBase.h"

#include "Proj.h"

GunEffectVariant AGunBase::Fire(FRotator CharacterRotation)
{
	OnFired();

	return {};
}

void AGunBase::TriggerClientVFX(FRotator CharacterRotation)
{
	ClientPlayVFX(CharacterRotation, nullptr);
}

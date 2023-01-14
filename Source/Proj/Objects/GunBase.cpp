// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/GunBase.h"

#include "Proj.h"

void AGunBase::Fire()
{
	OnFired();
	LOG("fired!")
}

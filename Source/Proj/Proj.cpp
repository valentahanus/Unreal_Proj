// Copyright Epic Games, Inc. All Rights Reserved.

#include "Proj.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Proj, "Proj" );

FString MyNetModeToString(ENetMode Mode)
{
	switch (Mode)
	{
	case NM_Standalone: return TEXT("Standalone");
	case NM_DedicatedServer: return TEXT("DedicatedServer");
	case NM_ListenServer: return TEXT("ListenServer");
	case NM_Client: return TEXT("Client");
	default: return TEXT("InvalidMode");
	}
}

FString NetRoleToString(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_SimulatedProxy: return TEXT("SimulatedProxy");
	case ROLE_AutonomousProxy: return TEXT("AutonomousProxy");
	case ROLE_Authority: return TEXT("Authority");
	default: return TEXT("InvalidRole");
	}
}

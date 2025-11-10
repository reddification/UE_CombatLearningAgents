// Copyright Epic Games, Inc. All Rights Reserved.

#include "NPC_ML.h"

#define LOCTEXT_NAMESPACE "FNPC_MLModule"

void FNPC_MLModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FNPC_MLModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNPC_MLModule, NPC_ML)
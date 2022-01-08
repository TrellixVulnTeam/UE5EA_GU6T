// Copyright Epic Games, Inc. All Rights Reserved.


#include "MMOARPGGateServer.h"

#include "RequiredProgramMainCPPInclude.h"

DEFINE_LOG_CATEGORY_STATIC(LogMMOARPGGateServer, Log, All);

IMPLEMENT_APPLICATION(MMOARPGGateServer, "MMOARPGGateServer");

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);
	UE_LOG(LogMMOARPGGateServer, Display, TEXT("Hello World"));
	FEngineLoop::AppExit();
	return 0;
}

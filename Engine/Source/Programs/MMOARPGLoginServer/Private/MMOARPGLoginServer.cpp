// Copyright Epic Games, Inc. All Rights Reserved.


#include "MMOARPGLoginServer.h"

#include "RequiredProgramMainCPPInclude.h"

DEFINE_LOG_CATEGORY_STATIC(LogMMOARPGLoginServer, Log, All);

IMPLEMENT_APPLICATION(MMOARPGLoginServer, "MMOARPGLoginServer");

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);
	UE_LOG(LogMMOARPGLoginServer, Display, TEXT("Hello World"));
	FEngineLoop::AppExit();
	return 0;
}

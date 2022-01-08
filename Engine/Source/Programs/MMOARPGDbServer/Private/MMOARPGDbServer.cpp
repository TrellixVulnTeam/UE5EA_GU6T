// Copyright Epic Games, Inc. All Rights Reserved.


#include "MMOARPGDbServer.h"

#include "RequiredProgramMainCPPInclude.h"

DEFINE_LOG_CATEGORY_STATIC(LogMMOARPGDbServer, Log, All);

IMPLEMENT_APPLICATION(MMOARPGDbServer, "MMOARPGDbServer");

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);
	UE_LOG(LogMMOARPGDbServer, Display, TEXT("Hello World"));
	FEngineLoop::AppExit();
	return 0;
}
